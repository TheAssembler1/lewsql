#ifndef REPLACEMENT_ALG_H
#define REPLACEMENT_ALG_H

#include<vector>
#include <stdint.h>
#include <memory>

#include "buffer_manager_types.h"

class ReplacementAlg {
public:
    ReplacementAlg(unsigned int num_pages) : num_pages{num_pages} {}

    virtual void on_pin(BufferPageCursor page_cursor, bool first) = 0;
    virtual void on_unpin(BufferPageCursor page_cursor) = 0;
    virtual BufferPageCursor get_victim(std::vector<bool>& pool_bitmap, std::unique_ptr<uint8_t[]>& mem_pool, unsigned int page_size) = 0;

    const unsigned int num_pages;
};

#endif // REPLACEMENT_ALG_H
