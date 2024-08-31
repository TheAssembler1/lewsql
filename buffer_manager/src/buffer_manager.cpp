#include "buffer_manager.h"

#include <disk_manager_types.h>

#include "buffer_manager_error.h"
#include "buffer_page.h"

void BufferManager::add_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor buffer_page_cursor) {
    auto map_iter = mem_pool_map.find(disk_id);

    if(map_iter == mem_pool_map.end()) {
        mem_pool_map[disk_id] = std::unordered_map<DiskId, DiskPageCursor>();
    }

    mem_pool_map[disk_id][disk_page_cursor] = buffer_page_cursor;
}

void BufferManager::remove_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    auto map_disk_iter = mem_pool_map.find(disk_id);
    assert(map_disk_iter != mem_pool_map.end());

    auto map_disk_page_iter = mem_pool_map[disk_id].find(disk_page_cursor);
    assert(map_disk_page_iter != mem_pool_map[disk_id].end());

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

BufferPage* BufferManager::get_page_mem_pool(BufferPageCursor buffer_page_cursor) const {
    assert(buffer_page_cursor <= num_pages);
    return reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor * disk_manager->page_size]);
}

std::ostream& BufferManager::print_bitmap(std::ostream& os, const BufferManager& buffer_manager) {
    unsigned int index = 0;
    while(index < buffer_manager.mem_pool_bitmap.size()) {
        if(buffer_manager.mem_pool_bitmap[index]) {
            os << "1";
        } else {
            os << "0";
        }
        index++;
        if(index == buffer_manager.mem_pool_bitmap.size()) {
            break;
        }
        if(index && index % 32 == 0) {
            os << std::endl;
        } else {
            os << ":";
        }
    }

    return os;
}

std::ostream& BufferManager::print_mem_pool_map(std::ostream& os, const BufferManager& buffer_manager) {
    for(BufferPageCursor cursor = 0; cursor < buffer_manager.num_pages; cursor++) {
        if(buffer_manager.mem_pool_bitmap[cursor]) {
            BufferPage* page = buffer_manager.get_page_mem_pool(cursor);

            if(cursor + 1 != buffer_manager.num_pages) {
                std::cout << *page << std::endl;
            }
        }
    }

    return os;
}

void BufferManager::free_avail_pages() {
    std::cout << "freeing all available pages" << std::endl;

    try {
        while(true) {
            auto victim = replacement_alg->get_victim(mem_pool_bitmap, mem_pool);
            BufferPage* buffer_page = get_page_mem_pool(victim);

            mem_pool_bitmap[victim] = false;
            remove_page_mem_pool_map(buffer_page->disk_id, buffer_page->disk_page_cursor);

            std::cout << "freeing avail page: " << victim << std::endl;
        }
    } catch(BufferManagerError& e) {
        assert(e.error_code == BufferManagerErrorCode::OUT_OF_PAGES);
    }
}

BufferPageCursor BufferManager::get_next_free_page() const {
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

    BufferPageCursor buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
    BufferPage* buffer_page = get_page_mem_pool(buffer_page_cursor);

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
        buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
        std::cout << "found page in cache" << std::endl;

        buffer_page = get_page_mem_pool(buffer_page_cursor);
        buffer_page->pin_count++;

        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        return &mem_pool[buffer_page_cursor];

        // NOTE: page not in cache yet
    } catch(std::exception& e) {
        try {
            buffer_page_cursor = get_next_free_page();
            std::cout << "found free page in bitmap: " << buffer_page_cursor << std::endl;
            buffer_page = get_page_mem_pool(buffer_page_cursor);
            // NOTE: no pages available need to evict
        } catch(BufferManagerError& e) {
            buffer_page_cursor = replacement_alg->get_victim(mem_pool_bitmap, mem_pool);

            BufferPage* victim_page = get_page_mem_pool(buffer_page_cursor);
            std::cout << "found victim page: " << victim_page << std::endl;
            if(victim_page->dirty) {
                std::cout << "victim page was dirty... flushing" << std::endl;
                disk_manager->d_write(disk_id, disk_page_cursor, victim_page->bytes);
            }
            // NOTE: remove victim page from map
            std::cout << "victim (disk_id, disk_page_cursor) = (" << victim_page->disk_id << ", "
                      << victim_page->disk_page_cursor << ")" << std::endl;
            remove_page_mem_pool_map(victim_page->disk_id, victim_page->disk_page_cursor);

            buffer_page = get_page_mem_pool(buffer_page_cursor);
        }

        buffer_page->init(disk_id, disk_page_cursor, buffer_page_cursor, 1, false);
        disk_manager->d_read(disk_id, disk_page_cursor, buffer_page->bytes);

        std::cout << "pin resulted in pin count: " << buffer_page->pin_count << std::endl;

        add_page_mem_pool_map(disk_id, disk_page_cursor, buffer_page_cursor);
        mem_pool_bitmap[buffer_page_cursor] = true;

        return &(*get_page_mem_pool(buffer_page_cursor)->bytes);
    }
}

void BufferManager::set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor) {
    std::cout << "setting dirty page: (disk_id, disk_page_cursor) = (" << disk_id << ", " << disk_page_cursor << ")" << std::endl;
    BufferPageCursor buffer_page_cursor = get_page_mem_pool_map(disk_id, disk_page_cursor);
    BufferPage* buffer_page = get_page_mem_pool(buffer_page_cursor);
    buffer_page->dirty = true;
}
