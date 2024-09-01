#ifndef BUFFER_PAGE_TRACKER
#define BUFFER_PAGE_TRACKER

#include <vector>
#include <iostream>

#include "buffer_manager_types.h"

class BufferPageTracker {
public:
    BufferPageTracker(unsigned int num_pages): num_pages(num_pages) {}

    virtual BufferPageCursor get_next_free_page() const = 0;

    virtual void set_page_status(BufferPageCursor buffer_page_cursor, bool taken) = 0;
    virtual bool get_page_status(BufferPageCursor buffer_page_cursor) const = 0;

    virtual unsigned int get_num_free_pages() const = 0;
    virtual unsigned int get_num_taken_pages() const = 0;
protected:
    unsigned int num_pages;
};

#endif // BUFFER_PAGE_TRACKER
