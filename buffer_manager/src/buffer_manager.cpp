#include "buffer_manager.h"

#include <disk_manager_types.h>

#include "buffer_page.h"
#include "buffer_manager_error.h"

static BufferPageCursor get_page_cursor(MemPoolMapType mem_pool_map, DiskId disk_id, DiskPageCursor disk_page_cursor) {
    auto disk_iter = mem_pool_map.find(disk_id);

    if(disk_iter == mem_pool_map.end()) {
        throw BufferManagerError(BufferManagerErrorCode::DISK_NOT_FOUND);
    }

    auto disk_page_iter = disk_iter->second.find(disk_page_cursor);

    if(disk_page_iter == disk_iter->second.end()) {
        throw BufferManagerError(BufferManagerErrorCode::DISK_NOT_FOUND);
    }

    return disk_page_iter->second;
}

BufferPageCursor BufferManager::get_next_free_page() {
    BufferPageCursor cursor = 0;

    while(mem_pool_bitmap[cursor]) {
        cursor++;
    }

    if(cursor == mem_pool_bitmap.size()) {
        throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGES);
    }

    return cursor;
}

void BufferManager::unpin(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    BufferPageCursor buffer_page_cursor = get_page_cursor(mem_pool_map, disk_id, disk_page_cursor);

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
    buffer_page->pin_count--;
}

uint8_t* BufferManager::pin(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    BufferPageCursor buffer_page_cursor;

    // NOTE: page is in cache
    try {
        buffer_page_cursor = get_page_cursor(mem_pool_map, disk_id, disk_page_cursor);
    // NOTE: page not in cache yet
    } catch(std::exception& e) {
        try {
            buffer_page_cursor = get_next_free_page();
        // NOTE: no pages available need to evict
        } catch(std::exception& e) {
            buffer_page_cursor = replacement_alg->get_victim();

            BufferPage* victim_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
            if(victim_page->dirty) {
                disk_manager->d_write(disk_id, disk_page_cursor, &mem_pool[buffer_page_cursor]);
            }
        }

        // NOTE: overwrite existing page
        disk_manager->d_read(disk_id, disk_page_cursor, &mem_pool[buffer_page_cursor]);
    }

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
    buffer_page->pin_count++;

    return &mem_pool[buffer_page_cursor];
}

void BufferManager::set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    BufferPageCursor buffer_page_cursor = get_page_cursor(mem_pool_map, disk_id, disk_page_cursor);

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
    buffer_page->dirty = true;
}
