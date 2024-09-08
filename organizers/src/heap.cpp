#include "heap.h"
#include "constants.h"

#include <buffer_manager_types.h>
#include <cassert>
#include <optional>
#include <bitmap.h>
#include <logger.h>

#define ROOT_PAGE (0)

#define FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET (0)
#define FIRST_PAGE_FREE_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP))
#define FIRST_PAGE_FULL_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP) + sizeof(int32_t))

#define FREE_LIST_OFFSET (0)
#define FULL_LIST_OFFSET (sizeof(int32_t))
#define BITMAP_OFFSET (sizeof(int32_t) + sizeof(int32_t))
#define FIRST_TUPLE_OFFSET(bitmap_size) (sizeof(int32_t) + sizeof(int32_t) + (bitmap_size * sizeof(uint8_t)))

// FIMXE: move to buffer manager
template<typename T>
static T* to_ptr(uint8_t* from) {
    return reinterpret_cast<T*>(from);
}

// NOTE: page 0 has
Heap::Heap(std::shared_ptr<DiskManager::DiskManager> disk_manager,
std::shared_ptr<BufferManager> buffer_manager,
std::string table_name,
TupleCols cols,
unsigned int page_size)
: disk_manager{disk_manager}, buffer_manager{buffer_manager}, table_name{table_name}, cols{cols}, page_size{page_size} {
    try {
        disk_id = disk_manager->create(table_name);

        LOG(LogLevel::INFO) << "created table with name: " << table_name << std::endl;
    } catch(DiskManagerError& e) {
        assert(e.error_code == DiskManagerErrorCode::DISK_ALREADY_EXISTS);

        disk_id = disk_manager->load(table_name);

        LOG(LogLevel::INFO) << "table already existed loading table with name: " << table_name << std::endl;
    }

    BufferPage& root_page = buffer_manager->pin(disk_id, ROOT_PAGE);

    to_ptr<uint32_t>(root_page.bytes)[FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET] = HEAP_PAGE_START_STAMP;
    to_ptr<int32_t>(root_page.bytes)[FIRST_PAGE_FREE_LIST_OFFSET] = -1;
    to_ptr<int32_t>(root_page.bytes)[FIRST_PAGE_FULL_LIST_OFFSET] = -1;

    buffer_manager->set_dirty(disk_id, ROOT_PAGE);
    buffer_manager->unpin(disk_id, ROOT_PAGE);
};

// FIXME: currently just pulling the first free page this won't be scalable
//        when needing to add large amounts of tuples
std::optional<unsigned int> Heap::find_free_page() {
    BufferPage* root_page = &buffer_manager->pin(disk_id, ROOT_PAGE);
    int32_t free_list = to_ptr<int32_t>(root_page->bytes)[FIRST_PAGE_FREE_LIST_OFFSET];
    buffer_manager->unpin(disk_id, ROOT_PAGE);

    if(free_list == -1) {
        return std::nullopt;
    }

    return free_list;
}

void Heap::append_to_full_pages(DiskPageCursor disk_page_cursor) {
    DiskPageCursor cur_page_cursor = ROOT_PAGE;
    int full_list_offset = 0;

    BufferPage* cur_page = &buffer_manager->pin(disk_id, ROOT_PAGE);

    if(to_ptr<int32_t>(cur_page->bytes)[FIRST_PAGE_FULL_LIST_OFFSET] == -1) {
        full_list_offset = FIRST_PAGE_FULL_LIST_OFFSET;
    } else {
        // NOTE: unpinning root page
        cur_page_cursor = to_ptr<int32_t>(cur_page->bytes)[FIRST_PAGE_FULL_LIST_OFFSET];
        buffer_manager->unpin(disk_id, ROOT_PAGE);

        for(;;) {
            cur_page = &buffer_manager->pin(disk_id, cur_page_cursor);

            if(to_ptr<int32_t>(cur_page->bytes)[FULL_LIST_OFFSET] == -1) {
                full_list_offset = FULL_LIST_OFFSET;
                break;
            } else {
                DiskPageCursor next_cur_page_cursor = to_ptr<int32_t>(cur_page->bytes)[FULL_LIST_OFFSET];
                buffer_manager->unpin(disk_id, cur_page_cursor);
                cur_page_cursor = next_cur_page_cursor;
            }
        }
    }

    // NOTE: cur_page should be a pointer to the currently pinned page which is last in the full list
    //       cur_page_cursor is the last page in the full list
    //       full_list_offset is the offset of the pointer to the next full list page
    //       it should be set to -1 currently
    to_ptr<int32_t>(cur_page->bytes)[full_list_offset] = disk_page_cursor;
    buffer_manager->set_dirty(disk_id, cur_page_cursor);
    buffer_manager->unpin(disk_id, cur_page_cursor);
}

void Heap::get_next_tuple() const {

}

void Heap::insert_tuple(const Tuple& tuple) {
    LOG(LogLevel::INFO) << "starting insertion for tuple" << std::endl;

    auto free_page_opt = find_free_page();
    DiskPageCursor free_disk_page;

    // NOTE: no free pages need to append to free list
    if(!free_page_opt.has_value()) {
        free_disk_page = disk_manager->extend(disk_id);
        BufferPage& root_page = buffer_manager->pin(disk_id, ROOT_PAGE);

        to_ptr<uint32_t>(root_page.bytes)[FIRST_PAGE_FREE_LIST_OFFSET] = disk_id;

        buffer_manager->set_dirty(disk_id, ROOT_PAGE);
        buffer_manager->unpin(disk_id, ROOT_PAGE);
    } else {
        free_disk_page = free_page_opt.value();
    }

    assert(free_disk_page != ROOT_PAGE);
    BufferPage& free_page = buffer_manager->pin(disk_id, free_disk_page);

    // NOTE: get free tuple offset from bitmap and set bit and serialize to buffer page
    unsigned int bitmap_size = page_size - (sizeof(int32_t) + sizeof(int32_t)) + (sizeof(uint8_t) * 57);
    LOG(LogLevel::INFO) << "setting bitmap size to: " << bitmap_size << std::endl;

    Bitmap bitmap{bitmap_size};
    bitmap.deserialize(&free_page.bytes[BITMAP_OFFSET]);
    assert(bitmap.num_free_bits() > 0);
    std::optional<unsigned int> tuple_offset_opt = bitmap.get_first_free_bit();
    assert(tuple_offset_opt.has_value());
    bitmap.set_bit_val(tuple_offset_opt.value(), true);
    bitmap.serialize(&free_page.bytes[BITMAP_OFFSET]);

    LOG(LogLevel::INFO) << "found free bit at: " << tuple_offset_opt.value() << std::endl;

    // NOTE: write tuple at found available offset
    unsigned int tuple_offset_within_page = FIRST_TUPLE_OFFSET(bitmap.get_num_bytes()) + (tuple_offset_opt.value() * tuple.size());
    LOG(LogLevel::INFO) << "writing tuple to offset within page: " << tuple_offset_within_page << std::endl;
    tuple.serialize(&free_page.bytes[tuple_offset_within_page]);

    // NOTE: checking if this is no longer a free page
    if(bitmap.num_free_bits() == 0) {
        LOG(LogLevel::INFO) << "no free bits in bitmap adding to full list" << std::endl;
        append_to_full_pages(free_disk_page);
    }

    buffer_manager->set_dirty(disk_id, free_disk_page);
    buffer_manager->unpin(disk_id, free_disk_page);

    LOG(LogLevel::INFO) << "done inserting tuple" << std::endl;
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
