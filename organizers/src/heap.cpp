#include "heap.h"
#include "constants.h"

#include <bitmap.h>
#include <buffer_manager_types.h>
#include <cassert>
#include <logger.h>
#include <optional>

#define HEAP_DISK_SUFFIX ".heap"

#define ROOT_PAGE (0)

#define FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET (0)
#define FIRST_PAGE_FREE_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP))
#define FIRST_PAGE_FULL_LIST_OFFSET (sizeof(HEAP_PAGE_START_STAMP) + sizeof(int32_t))

#define FREE_LIST_OFFSET (0)
#define FULL_LIST_OFFSET (sizeof(int32_t))
#define BITMAP_OFFSET (sizeof(int32_t) + sizeof(int32_t))
#define FIRST_TUPLE_OFFSET(bitmap_size) (sizeof(int32_t) + sizeof(int32_t) + (bitmap_size * sizeof(uint8_t)))

// NOTE: page 0 has
Heap::Heap(std::shared_ptr<DiskManager::DiskManager> disk_manager,
std::shared_ptr<BufferManager> buffer_manager,
std::string table_name,
TupleCols cols,
unsigned int page_size)
: m_disk_manager{disk_manager}, m_buffer_manager{buffer_manager}, m_table_name{table_name + HEAP_DISK_SUFFIX}, cols{cols}, page_size{page_size} {
    m_disk_id = disk_manager->create(m_table_name).value_or([&](DiskManagerError error) {
        return static_cast<DiskId>(disk_manager->load(m_table_name).get_value());
    });

    BufferPage& root_page = m_buffer_manager->pin(m_disk_id, ROOT_PAGE);

    root_page.to_ptr<uint32_t>(FIRST_PAGE_HEAP_PAGE_STAMP_OFFSET)[0] = HEAP_PAGE_START_STAMP;
    root_page.to_ptr<int32_t>(FIRST_PAGE_FREE_LIST_OFFSET)[0] = -1;
    root_page.to_ptr<int32_t>(FIRST_PAGE_FULL_LIST_OFFSET)[0] = -1;

    m_buffer_manager->set_dirty(m_disk_id, ROOT_PAGE);
    m_buffer_manager->unpin(m_disk_id, ROOT_PAGE);
};

// FIXME: currently just pulling the first free page this won't be scalable
//        when needing to add large amounts of tuples
std::optional<unsigned int> Heap::find_free_page() {
    BufferPage* root_page = &m_buffer_manager->pin(m_disk_id, ROOT_PAGE);
    int32_t free_list = root_page->to_ptr<int32_t>(FIRST_PAGE_FREE_LIST_OFFSET)[0];
    m_buffer_manager->unpin(m_disk_id, ROOT_PAGE);

    if(free_list == -1) {
        return std::nullopt;
    }

    return free_list;
}

void Heap::append_to_full_pages(DiskPageCursor disk_page_cursor) {
    DiskPageCursor cur_page_cursor = ROOT_PAGE;
    int full_list_offset = 0;

    BufferPage* cur_page = &m_buffer_manager->pin(m_disk_id, ROOT_PAGE);

    if(cur_page->to_ptr<int32_t>(FIRST_PAGE_FULL_LIST_OFFSET)[0] == -1) {
        full_list_offset = FIRST_PAGE_FULL_LIST_OFFSET;
    } else {
        // NOTE: unpinning root page
        cur_page_cursor = cur_page->to_ptr<int32_t>(FIRST_PAGE_FULL_LIST_OFFSET)[0];
        m_buffer_manager->unpin(m_disk_id, ROOT_PAGE);

        for(;;) {
            cur_page = &m_buffer_manager->pin(m_disk_id, cur_page_cursor);

            if(cur_page->to_ptr<int32_t>(FULL_LIST_OFFSET)[0] == -1) {
                full_list_offset = FULL_LIST_OFFSET;
                break;
            } else {
                DiskPageCursor next_cur_page_cursor = cur_page->to_ptr<int32_t>(FULL_LIST_OFFSET)[0];
                m_buffer_manager->unpin(m_disk_id, cur_page_cursor);
                cur_page_cursor = next_cur_page_cursor;
            }
        }
    }

    // NOTE: cur_page should be a pointer to the currently pinned page which is last in the full list
    //       cur_page_cursor is the last page in the full list
    //       full_list_offset is the offset of the pointer to the next full list page
    //       it should be set to -1 currently
    cur_page->to_ptr<int32_t>(full_list_offset)[0] = disk_page_cursor;
    m_buffer_manager->set_dirty(m_disk_id, cur_page_cursor);
    m_buffer_manager->unpin(m_disk_id, cur_page_cursor);
}

void Heap::get_next_tuple() const {
}

void Heap::insert_tuple(const Tuple& tuple) {
    LOG(LogLevel::INFO) << "starting insertion for tuple" << std::endl;

    auto free_page_opt = find_free_page();
    DiskPageCursor free_disk_page;

    // NOTE: no free pages need to append to free list
    if(!free_page_opt.has_value()) {
        free_disk_page = m_disk_manager->extend(m_disk_id).get_value();
        BufferPage& root_page = m_buffer_manager->pin(m_disk_id, ROOT_PAGE);

        root_page.to_ptr<int32_t>(FIRST_PAGE_FREE_LIST_OFFSET)[0] = m_disk_id;

        m_buffer_manager->set_dirty(m_disk_id, ROOT_PAGE);
        m_buffer_manager->unpin(m_disk_id, ROOT_PAGE);
    } else {
        free_disk_page = free_page_opt.value();
    }

    assert(free_disk_page != ROOT_PAGE);
    BufferPage& free_page = m_buffer_manager->pin(m_disk_id, free_disk_page);

    // NOTE: get free tuple offset from bitmap and set bit and serialize to buffer page
    unsigned int bitmap_size = page_size - (sizeof(int32_t) + sizeof(int32_t)) + (sizeof(uint8_t) * 57);
    LOG(LogLevel::INFO) << "setting bitmap size to: " << bitmap_size << std::endl;

    Bitmap bitmap{bitmap_size};
    bitmap.deserialize(&free_page.bytes[BITMAP_OFFSET]);
    assert(bitmap.get_num_free_bits() > 0);
    std::optional<unsigned int> tuple_offset_opt = bitmap.get_first_free_bit();
    assert(tuple_offset_opt.has_value());
    bitmap.set_bit_val(tuple_offset_opt.value(), true);
    bitmap.serialize(&free_page.bytes[BITMAP_OFFSET]);

    LOG(LogLevel::INFO) << "found free bit at: " << tuple_offset_opt.value() << std::endl;

    // NOTE: write tuple at found available offset
    unsigned int tuple_offset_within_page =
    FIRST_TUPLE_OFFSET(bitmap.get_num_bytes()) + (tuple_offset_opt.value() * tuple.size());
    LOG(LogLevel::INFO) << "writing tuple to offset within page: " << tuple_offset_within_page << std::endl;
    tuple.serialize(&free_page.bytes[tuple_offset_within_page]);

    // NOTE: checking if this is no longer a free page
    if(bitmap.get_num_free_bits() == 0) {
        LOG(LogLevel::INFO) << "no free bits in bitmap adding to full list" << std::endl;
        append_to_full_pages(free_disk_page);
    }

    m_buffer_manager->set_dirty(m_disk_id, free_disk_page);
    m_buffer_manager->unpin(m_disk_id, free_disk_page);

    LOG(LogLevel::INFO) << "done inserting tuple" << std::endl;
}

void Heap::delete_tuple() {
}
