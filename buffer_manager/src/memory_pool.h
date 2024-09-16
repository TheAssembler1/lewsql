#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstring>
#include <logger.h>
#include <memory>

#include "buffer_page.h"

class MemoryPool {
    public:
    MemoryPool(unsigned int num_pages, unsigned int page_size)
    : num_pages{num_pages}, page_size{page_size},
      // NOTE: allocates pages plus BufferPage memers into a mem pool
      pool{new uint8_t[(sizeof(BufferPage) + page_size) * num_pages]} {
        assert(num_pages > 0 && page_size > 0);
        LOG(LogLevel::INFO)
        << "creating memory pool with (num_pages, page_size) = (" << num_pages << ", " << page_size << ")" << std::endl;
        LOG(LogLevel::INFO) << "memory pool size: " << num_pages * page_size << std::endl;
        std::memset(static_cast<void*>(&pool[0]), 0, BufferPage::size(page_size) * num_pages);
    }

    BufferPage* get_page(BufferPageCursor buffer_page_cursor) const;

    const unsigned int num_pages;
    const unsigned int page_size;

    // NOTE: mem pool with buffer pages
    std::unique_ptr<uint8_t[]> pool;
};

#endif // MEMORY_POOL_H
