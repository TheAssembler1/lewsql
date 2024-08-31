#ifndef DUMB_ALG_H
#define DUMB_ALG_H

#include "replacement_alg.h"

class DumbAlg: public ReplacementAlg {
public:
    DumbAlg(unsigned int num_pages): ReplacementAlg{num_pages} {}

    virtual void on_pin(BufferPageCursor page_cursor, bool first) override;
    virtual void on_unpin(BufferPageCursor page_cursor) override;
    virtual BufferPageCursor get_victim(std::vector<bool>& pool_bitmap, std::unique_ptr<uint8_t[]>& mem_pool) override;
};

#endif // DUMB_ALG_H
