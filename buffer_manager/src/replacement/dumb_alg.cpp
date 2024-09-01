#include "dumb_alg.h"
#include "buffer_manager_error.h"
#include "buffer_page.h"

void DumbAlg::on_pin(BufferPageCursor page_cursor) {
}
void DumbAlg::on_unpin(BufferPageCursor page_cursor) {
}

BufferPageCursor DumbAlg::get_victim(const BufferPageTracker& buffer_page_tracker, const MemoryPool& mem_pool) {
    BufferPageCursor buffer_page_cursor = 0;
    for(int i = 0; i < mem_pool.num_pages; i++) {
        if(buffer_page_tracker.get_page_status(buffer_page_cursor)) {
            BufferPage* buffer_page = mem_pool.get_page(buffer_page_cursor);
            if(buffer_page->pin_count == 0) {
                return buffer_page_cursor;
            }
        }

        buffer_page_cursor++;
    }


    throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGES);
}
