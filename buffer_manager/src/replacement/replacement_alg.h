#ifndef REPLACEMENT_ALG_H
#define REPLACEMENT_ALG_H

#include <memory>
#include <stdint.h>
#include <vector>

#include "buffer_manager_types.h"
#include "buffer_page_tracker/buffer_page_tracker.h"

class ReplacementAlg {
    public:
    virtual void on_pin(BufferPageCursor page_cursor) = 0;
    virtual void on_unpin(BufferPageCursor page_cursor) = 0;
    virtual BufferPageCursor get_victim(const BufferPageTracker& buffer_page_tracker) = 0;
};

#endif // REPLACEMENT_ALG_H
