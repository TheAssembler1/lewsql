#ifndef DUMB_ALG_H
#define DUMB_ALG_H

#include "replacement_alg.h"

class DumbAlg : public ReplacementAlg {
    public:
    virtual void on_pin(BufferPageCursor page_cursor) override;
    virtual void on_unpin(BufferPageCursor page_cursor) override;
    virtual BufferPageCursor get_victim(const BufferPageTracker& buffer_page_tracker) override;
};

#endif // DUMB_ALG_H
