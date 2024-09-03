#ifndef BITMAP_TRACKER_H
#define BITMAP_TRACKER_H

#include "buffer_manager_error.h"
#include "buffer_manager_types.h"
#include "buffer_page_tracker.h"

class BitmapTracker final : public BufferPageTracker {
    public:
    BitmapTracker(unsigned int num_pages) : BufferPageTracker(num_pages), bitmap(num_pages){};

    virtual BufferPageCursor get_next_free_page() const override;

    virtual void set_page_status(BufferPageCursor buffer_page_cursor, bool taken) override;
    virtual bool get_page_status(BufferPageCursor buffer_page_cursor) const override;

    virtual unsigned int get_num_free_pages() const override;
    virtual unsigned int get_num_taken_pages() const override;

    friend std::ostream& operator<<(std::ostream& os, const BitmapTracker& bitmap_tracker);

    private:
    // NOTE: tracks allocated pages
    std::vector<bool> bitmap;
};

#endif // BITMAP_TRACKER_H
