#include "buffer_manager.h"

#include <disk_manager_types.h>

#include "buffer_manager_error.h"
#include "buffer_page.h"

void BufferManager::add_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor buffer_page_cursor) {
    std::cout << "adding page to mem pool at: " << buffer_page_cursor << std::endl;

    auto map_iter = mem_pool_map.find(disk_id);

    if(map_iter == mem_pool_map.end()) {
        mem_pool_map[disk_id] = std::unordered_map<DiskId, DiskPageCursor>();
    }

    buffer_page_tracker->set_page_status(buffer_page_cursor, true);
    mem_pool_map[disk_id][disk_page_cursor] = buffer_page_cursor;
}

void BufferManager::remove_page_mem_pool_map(BufferPageCursor victim_page_cursor) {
    std::cout << "removing page from mem pool: " << victim_page_cursor << std::endl;

    BufferPage* buffer_page = mem_pool.get_page(victim_page_cursor);

    assert(buffer_page->pin_count == 0);

    DiskId disk_id =buffer_page->disk_id;
    DiskPageCursor disk_page_cursor = buffer_page->disk_page_cursor;
    bool dirty = buffer_page->dirty;
    uint8_t* bytes = buffer_page->bytes;

    auto map_disk_iter = mem_pool_map.find(disk_id);
    assert(map_disk_iter != mem_pool_map.end());

    auto map_disk_page_iter = mem_pool_map[disk_id].find(disk_page_cursor);
    assert(map_disk_page_iter != mem_pool_map[disk_id].end());

    if(dirty) {
        std::cout << "victim was dirty" << std::endl;
        disk_manager->d_write(disk_id, disk_page_cursor, bytes, page_size);
    }

    buffer_page_tracker->set_page_status(victim_page_cursor, false);
    mem_pool_map[disk_id].erase(disk_page_cursor);

    // NOTE: only erase disk if there are no other disk page cursors
    if(!mem_pool_map[disk_id].size()) {
        mem_pool_map.erase(disk_id);
    }
}

BufferPageCursor BufferManager::get_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor) const {
    std::cout << "getting page cursor: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    auto disk_iter = mem_pool_map.find(disk_id);

    // NOTE: this is an expected error in some cases
    if(disk_iter == mem_pool_map.end()) {
        throw BufferManagerError(BufferManagerErrorCode::DISK_NOT_FOUND);
    }

    auto disk_page_iter = disk_iter->second.find(disk_page_cursor);

    // NOTE: this is an expected error in some cases
    if(disk_page_iter == disk_iter->second.end()) {
        throw BufferManagerError(BufferManagerErrorCode::DISK_PAGE_CURSOR_NOT_FOUND);
    }

    return disk_page_iter->second;
}

std::ostream& BufferManager::print_mem_pool_map(std::ostream& os, const BufferManager& buffer_manager) {
    for(BufferPageCursor cursor = 0; cursor < buffer_manager.mem_pool.num_pages; cursor++) {
        if(buffer_manager.buffer_page_tracker->get_page_status(cursor)) {
            BufferPage* page = buffer_manager.mem_pool.get_page(cursor);

            std::cout << *page;
            if(cursor + 1 < buffer_manager.buffer_page_tracker->get_num_taken_pages()) {
                std::cout << std::endl;
            }
        }
    }

    return os;
}

void BufferManager::free_avail_pages() {
    std::cout << "freeing all available pages" << std::endl;

    try {
        while(true) {
            auto victim = replacement_alg->get_victim(*buffer_page_tracker, mem_pool);
            remove_page_mem_pool_map(victim);

            std::cout << "freeing avail page: " << victim << std::endl;
        }
    } catch(BufferManagerError& e) {
        assert(e.error_code == BufferManagerErrorCode::OUT_OF_PAGES);
    }
}

void BufferManager::unpin(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "unpinning page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
    BufferPage* buffer_page = mem_pool.get_page(buffer_page_cursor);

    assert(buffer_page->pin_count);
    buffer_page->pin_count--;

    std::cout << "unpin resulted in pin count: " << buffer_page->pin_count << std::endl;
    replacement_alg->on_unpin(buffer_page_cursor);
}

BufferPage* BufferManager::pin(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "pinning page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor;
    BufferPage* buffer_page;
    // NOTE: page is in cache
    try {
        buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
        std::cout << "found page in cache" << std::endl;

        buffer_page = mem_pool.get_page(buffer_page_cursor);
        buffer_page->pin_count++;

        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        return mem_pool.get_page(buffer_page_cursor);

        // NOTE: page not in cache yet
    } catch(std::exception& e) {
        try {
            buffer_page_cursor = buffer_page_tracker->get_next_free_page();
            std::cout << "found free page in bitmap: " << buffer_page_cursor << std::endl;
            buffer_page = mem_pool.get_page(buffer_page_cursor);
            // NOTE: no pages available need to evict
        } catch(BufferManagerError& e) {
            BufferPageCursor victim_page = replacement_alg->get_victim(*buffer_page_tracker, mem_pool);
            std::cout << "found victim page: " << victim_page << std::endl;

            remove_page_mem_pool_map(victim_page);

            buffer_page_cursor = victim_page;
            buffer_page = mem_pool.get_page(victim_page);
        }

        buffer_page->init(disk_id, disk_page_cursor, buffer_page_cursor, 1, false);
        disk_manager->d_read(disk_id, disk_page_cursor, buffer_page->bytes, page_size);

        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        add_page_mem_pool_map(disk_id, disk_page_cursor, buffer_page_cursor);

        replacement_alg->on_pin(buffer_page_cursor);
        return buffer_page;
    }
}

void BufferManager::set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "setting dirty page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;

    BufferPageCursor buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
    BufferPage* buffer_page = mem_pool.get_page(buffer_page_cursor);
    buffer_page->dirty = true;
}
