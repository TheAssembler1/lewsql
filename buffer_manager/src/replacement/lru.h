#ifndef LRU_H
#define LRU_H

#include <queue>
#include <unordered_map>

#include "replacement_alg.h"

class LRU final : public ReplacementAlg {
public:
    virtual void on_pin(BufferPageCursor buffer_page_cursor, bool first) override;
    virtual void on_unpin(BufferPageCursor buffer_page_cursor) override;

    virtual BufferPageCursor get_victim(std::vector<bool>& pool_bitmap) override;

    unsigned int stamp{0};

    // NOTE: map stamp to page
    std::unordered_map<unsigned int, unsigned int> stamp_page_map{};
    std::priority_queue<unsigned int> queue{};
};

#endif
