#include "memory_pool.h"

#include <logger.h>

BufferPage* MemoryPool::get_page(BufferPageCursor buffer_page_cursor) const {
    LOG(LogLevel::INFO) << "getting mem pool page: " << buffer_page_cursor << std::endl;
    assert(buffer_page_cursor < num_pages);
    assert(pool);

    return reinterpret_cast<BufferPage*>(&pool[BufferPage::size(page_size) * buffer_page_cursor]);
}
