#include "heap.h"

#include <buffer_manager_types.h>

void Heap::push_back_record(Tuple tuple) {
    // NOTE: find last record
    BufferPageCursor cur_page_cursor = 0;
    BufferPageByteCursor cur_page_byte_cursor = 0;

    BufferPage& page = buffer_manager->pin(disk_id, cur_page_cursor);

    uint8_t* page_bytes = page.bytes;
    tuple.serialize(page_bytes, false);

    buffer_manager->set_dirty(disk_id, cur_page_cursor);
    buffer_manager->unpin(disk_id, cur_page_cursor);
};

bool Heap::valid_record() const {
}
