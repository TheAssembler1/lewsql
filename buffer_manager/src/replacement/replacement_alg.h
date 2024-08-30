#ifndef REPLACEMENT_ALG_H
#define REPLACEMENT_ALG_H

#include<vector>

#include "buffer_manager_types.h"

class ReplacementAlg {
public:
    virtual void on_pin(BufferPageCursor page_cursor, bool first) = 0;
    virtual void on_unpin(BufferPageCursor page_cursor) = 0;
    virtual BufferPageCursor get_victim(std::vector<bool>& pool_bitmap) = 0;
};

#endif // REPLACEMENT_ALG_H
