#include "dumb_alg.h"
#include "buffer_manager_error.h"
#include "buffer_page.h"

void DumbAlg::on_pin(BufferPageCursor page_cursor, bool first) {
}
void DumbAlg::on_unpin(BufferPageCursor page_cursor) {
}

BufferPageCursor DumbAlg::get_victim(std::vector<bool>& pool_bitmap, std::unique_ptr<uint8_t[]>& mem_pool) {
    BufferPageCursor buffer_page_cursor = 0;
    for(auto bit_page: pool_bitmap) {
        if(bit_page) {
            // FIXME: need a way to call buffer manager get page
            // lets seperate the buffer manager from the memory pool
            BufferPage* buffer_page = reinterpret_cast<BufferPage*>(&mem_pool[buffer_page_cursor * 512]);
            if(buffer_page->pin_count == 0) {
                return buffer_page_cursor;
            }
        }

        buffer_page_cursor++;
    }


    throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGES);
}
