#include "dumb_alg.h"
#include "buffer_manager_error.h"
#include "buffer_page.h"

void DumbAlg::on_pin(BufferPageCursor page_cursor, bool first) {
}
void DumbAlg::on_unpin(BufferPageCursor page_cursor) {
}

BufferPageCursor DumbAlg::get_victim(std::vector<bool>& pool_bitmap, std::unique_ptr<uint8_t[]>& mem_pool) {
    BufferPage* page = reinterpret_cast<BufferPage*>(mem_pool.get());

    for(BufferPageCursor i = 0; i < num_pages; i++) {
        if(!page[i].pin_count && page[i].dirty) {
            return i;
        }
    }

    throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGES);
}
