#include "heap.h"
#include "constants.h"

#include <buffer_manager_types.h>
#include <cassert>
#include <optional>

Heap::Heap(std::shared_ptr<DiskManager> disk_manager,
std::shared_ptr<BufferManager> buffer_manager,
std::string table_name,
TupleCols cols,
unsigned int page_size)
: disk_manager{disk_manager}, buffer_manager{buffer_manager}, table_name{table_name}, cols{cols}, page_size{page_size} {

    try {
        disk_id = disk_manager->d_create(table_name);

        std::cout << "created table with name: " << table_name << std::endl;
    } catch(DiskManagerError& e) {
        assert(e.error_code == DiskManagerErrorCode::DISK_ALREADY_EXISTS);

        disk_id = disk_manager->d_load(table_name);

        std::cout << "table already existed loading table with name: " << table_name << std::endl;
    }
};

void Heap::get_next_tuple() const {

}

void Heap::insert_tuple(const Tuple& tuple) {
    DiskPageCursor cur_page;
    bool in_free_list = false;

    // NOTE: get a page with free space
    if(free_pages.empty() && full_pages.empty()) {
        std::cout << "first page in heap" << std::endl;
        cur_page = 0;
    } else if(free_pages.empty()) {
        std::cout << "not free pages searching for slot in full pages" << std::endl;

        // NOTE: search for empty page slot in full list
        //       assumes the list is sorted
        for(int i = 0; i < full_pages.size(); i++) {
            if(i != full_pages[i]) {
                cur_page = i;
                break;
            }
        }

        // NOTE: grab next page, no gaps in free list
        std::cout << "not slot in full pages, getting next page" << std::endl;
        cur_page = full_pages.size();
    } else {
        std::cout << "found page in free pages" << std::endl;
        in_free_list = true;
        cur_page = free_pages[0];
    }

    BufferPage& page = buffer_manager->pin(disk_id, cur_page);
    unsigned int page_cursor = 0;

    if(cur_page == 0) {
        std::cout << "on page zero, moving foward by heap page stamp" << std::endl;
        uint32_t* heap_stamp = reinterpret_cast<uint32_t*>(&page.bytes[page_cursor]);
        *heap_stamp = HEAP_PAGE_START_STAMP;
        page_cursor += sizeof(HEAP_PAGE_START_STAMP);
    }

    uint32_t* next_free_tuple = reinterpret_cast<uint32_t*>(&page.bytes[page_cursor]);
    page_cursor += sizeof(uint32_t);

    std::cout << "serializing tuple at abs position: " << page_cursor + *next_free_tuple << std::endl;
    tuple.serialize(&page.bytes[page_cursor + *next_free_tuple]);

    *next_free_tuple += tuple.size();
    page_cursor += tuple.size();

    std::cout << "next free tuple set to: " << *next_free_tuple << std::endl;

    // NOTE: check if we have space for another tuple
    if(page_cursor + tuple.size() < page_size) {
        std::cout << "page still has more space" << std::endl;
        auto it = std::find(free_pages.begin(), free_pages.end(), cur_page);

        if(it != free_pages.end()) {
            std::cout << "page already in free list" << std::endl;
        } else {
            std::cout << "page not in free list, pushing to free list" << std::endl;
        }
    } else {
        std::cout << "no more space on page, erasing from free pages and pushing to full pages" << std::endl;
        auto it = std::find(free_pages.begin(), free_pages.end(), cur_page);
        assert(cur_page == 0 || it != free_pages.end());

        free_pages.erase(it);

        full_pages.push_back(cur_page);
    }

    // NOTE: set dirty and release page
    std::cout << "setting dirty and unpinning" << std::endl;
    buffer_manager->set_dirty(disk_id, cur_page);
    buffer_manager->unpin(disk_id, cur_page);

    std::cout << "sorting full pages pages" << std::endl;
    std::sort(full_pages.begin(), full_pages.end());
}

void Heap::delete_tuple() {

}
