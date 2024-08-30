#include "lru.h"

void LRU::on_pin(BufferPageCursor buffer_page_cursor, bool first) {
    if(first) {
        queue.push(stamp);
        stamp_page_map[stamp] = buffer_page_cursor;
        stamp++;
    }
}

// FIXME: only push to priroity queue on unpin
void LRU::on_unpin(BufferPageCursor buffer_page_cursor) { (void)buffer_page_cursor; }

BufferPageCursor LRU::get_victim(std::vector<bool>& pool_bitmap) {
    if(pool_bitmap[stamp_page_map[queue.top()]])
}
