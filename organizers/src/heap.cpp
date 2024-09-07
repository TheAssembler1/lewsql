#include "heap.h"
#include "constants.h"

#include <buffer_manager_types.h>
#include <cassert>
#include <optional>

#define FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET (0)
#define FIRST_PAGE_FREE_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP))
#define FIRST_PAGE_FULL_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP) + sizeof(int32_t))

#define FREE_LIST_OFFSET (0)
#define FULL_LIST_OFFSET (sizeof(int32_t))

// FIMXE: move to buffer manager
template<typename T>
static T* to_ptr(uint8_t* from) {
    return reinterpret_cast<T*>(from);
}

// NOTE: page 0 has
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

    BufferPage& root_page = buffer_manager->pin(disk_id, 0);

    to_ptr<uint32_t>(root_page.bytes)[FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET] = HEAP_PAGE_START_STAMP;
    to_ptr<int32_t>(root_page.bytes)[FIRST_PAGE_FREE_LIST_OFFSET] = -1;
    to_ptr<int32_t>(root_page.bytes)[FIRST_PAGE_FULL_LIST_OFFSET] = -1;

    buffer_manager->set_dirty(disk_id, 0);
    buffer_manager->unpin(disk_id, 0);
};

// FIXME: currently just pulling the first free page this won't be scalable
//        when needing to add large amounts of tuples
std::optional<unsigned int> Heap::find_free_page() {
    BufferPage* root_page = &buffer_manager->pin(disk_id, 0);
    int32_t free_list = to_ptr<int32_t>(root_page->bytes)[FIRST_PAGE_FREE_LIST_OFFSET];
    buffer_manager->unpin(disk_id, 0);

    if(free_list == -1) {
        return std::nullopt;
    }

    return free_list;
}

void Heap::get_next_tuple() const {

}

void Heap::insert_tuple(const Tuple& tuple) {
    auto free_page_opt = find_free_page();

    // NOTE: no free pages need to append to free list
    if(!free_page_opt.has_value()) {

    }
}

void Heap::delete_tuple() {

}

/*
 * std::optional<unsigned int> Heap::find_free_page() {
    BufferPageCursor cur_page = 0;
    BufferPage* root_page = &buffer_manager->pin(disk_id, cur_page);
    int32_t free_list = to_ptr<int32_t>(root_page->bytes)[FIRST_PAGE_FREE_LIST_OFFSET];
    buffer_manager->unpin(disk_id, cur_page);

    if(free_list == -1) {
        return std::nullopt;
    }

    for(;;) {
        root_page = &buffer_manager->pin(disk_id, cur_page);
        cur_page = to_ptr<int32_t>(root_page->bytes)[FREE_LIST_OFFSET];
        buffer_manager->unpin(disk_id, cur_page);

        if(free_list == -1) {
            return std::nullopt;
        }
    }
}*/
