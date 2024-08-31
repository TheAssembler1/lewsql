#include "buffer_manager.h"

#include <disk_manager_types.h>

#include "buffer_manager_error.h"
#include "buffer_page.h"

void BufferManager::free_avail_pages() {
    try {
        while(true) {
            auto victim = replacement_alg->get_victim(mem_pool_bitmap, mem_pool);
            std::cout << "freeing avail page: " << victim << std::endl;
        }
    } catch(BufferManagerError& e) {
        std::cout << e.what() << std::endl;
    }
}

BufferPageCursor BufferManager::get_page_cursor(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "getting page cursor: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

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

void BufferManager::set_page_cursor(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor bufer_page_cursor) {
    mem_pool_map[disk_id][disk_page_cursor] = bufer_page_cursor;
}

BufferPageCursor BufferManager::get_next_free_page() {
    std::cout << "getting next free page" << std::endl;

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
    std::cout << "unpinning page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor = get_page_cursor(disk_id, disk_page_cursor);

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);

    assert(buffer_page->pin_count);
    buffer_page->pin_count--;

    std::cout << "unpin resulted in pin count: " << buffer_page->pin_count << std::endl;
}

uint8_t* BufferManager::pin(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "pinning page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor;
    BufferPage* buffer_page;
    // NOTE: page is in cache
    try {
        buffer_page_cursor = get_page_cursor(disk_id, disk_page_cursor);

        std::cout << "found page in cache" << std::endl;
        buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
        buffer_page->pin_count++;
        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        return &mem_pool[buffer_page_cursor];
        // NOTE: page not in cache yet
    } catch(std::exception& e) {
        try {
            buffer_page_cursor = get_next_free_page();
            buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
            // NOTE: no pages available need to evict
        } catch(BufferManagerError& e) {
            buffer_page_cursor = replacement_alg->get_victim(mem_pool_bitmap, mem_pool);

            BufferPage* victim_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
            std::cout << "found victim page: " << victim_page << std::endl;

            if(victim_page->dirty) {
                std::cout << "victim page was dirty... flushing" << std::endl;
                disk_manager->d_write(disk_id, disk_page_cursor, &mem_pool[buffer_page_cursor]);
            }

            // NOTE: remove victim page from map
            mem_pool_map[victim_page->disk_id].erase(victim_page->disk_page_cursor);
            mem_pool_map.erase(victim_page->disk_id);

            buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
        }

        disk_manager->d_read(disk_id, disk_page_cursor, &mem_pool[buffer_page_cursor]);


        // NOTE: overwrite existing page
        buffer_page->disk_page_cursor = disk_page_cursor;
        buffer_page->disk_id = disk_id;
        buffer_page->dirty = false;
        buffer_page->pin_count = 1;

        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        set_page_cursor(buffer_page_cursor, disk_id, disk_page_cursor);
        mem_pool_bitmap[buffer_page_cursor] = true;
        return &mem_pool[buffer_page_cursor];
    }
}

void BufferManager::set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "setting dirty page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor = get_page_cursor(disk_id, disk_page_cursor);

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor]);
    buffer_page->dirty = true;
}
