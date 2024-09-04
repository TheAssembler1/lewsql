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

void Heap::push_back_tuple(Tuple tuple) {
    is_valid_tuple(tuple);

    // NOTE: find last record
    BufferPage* page = nullptr;
    BufferPageCursor cur_page_cursor = 0;
    unsigned int abs_byte_cursor = 0;
    bool was_last = false;

    for(;;) {
        // NOTE: skip past the first page heap stamp
        if(!abs_byte_cursor) {
            std::cout << "skipping heap page start stamp" << std::endl;
            abs_byte_cursor += sizeof(HEAP_PAGE_START_STAMP);
            page = &buffer_manager->pin(disk_id, cur_page_cursor);
        } else {
            std::cout << "not the starting byte" << std::endl;
        }

        // NOTE: check that tuple fits on page
        if((abs_byte_cursor % page_size) + (tuple.size() + sizeof(TUPLE_EXISTS_STAMP)) > page_size) {
            std::cout << "cross boundary tuple skipping bytes" << std::endl;

            // NOTE: skips to the beginning of the next page
            abs_byte_cursor += page_size - (abs_byte_cursor % page_size);
        }

        // NOTE: load the next page
        if(cur_page_cursor != abs_byte_cursor / page_size) {
            buffer_manager->unpin(disk_id, cur_page_cursor);
            cur_page_cursor = abs_byte_cursor / page_size;
            page = &buffer_manager->pin(disk_id, cur_page_cursor);
        }

        assert(page);

        if(!page->bytes[abs_byte_cursor % page_size]) {
            // NOTE: mark new last entry
            page->bytes[abs_byte_cursor % page_size] = TUPLE_EXISTS_STAMP;
            abs_byte_cursor += sizeof(TUPLE_EXISTS_STAMP);

            // NOTE: write new tuple after the last tuple byte
            std::cout << "writing tuple at abs_cursor: " << (abs_byte_cursor % page_size) << std::endl;
            tuple.serialize(&(page->bytes[(abs_byte_cursor % page_size)]));

            buffer_manager->set_dirty(disk_id, cur_page_cursor);
            buffer_manager->unpin(disk_id, cur_page_cursor);

            break;
        } else {
            std::cout << "found previous last entry abs_cursor: " << abs_byte_cursor % page_size << std::endl;
        }

        abs_byte_cursor += tuple.size() + sizeof(TUPLE_EXISTS_STAMP);
    }
};

// FIXME: throw instead of assertting the error here
bool Heap::is_valid_tuple(const Tuple& tuple) const {
    // NOTE: validating record
    assert(cols.size() == tuple.num_cells());
    int cur = 0;
    for(auto& col : cols) {
        assert(Type::get_type_name(col) == tuple.get_cell_type(cur++));
    }
}
