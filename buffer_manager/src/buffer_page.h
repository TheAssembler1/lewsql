#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <disk_manager.h>
#include <cstdint>
#include <memory>
#include <iostream>

#include "buffer_manager_types.h"

struct BufferPage {
    void init(DiskId _disk_id, DiskPageCursor _disk_page_cursor, BufferPageCursor _buffer_page_cursor, unsigned int _pin_count, bool _dirty) {
        disk_id = _disk_id;
        disk_page_cursor = _disk_page_cursor;
        buffer_page_cursor = _buffer_page_cursor;
        pin_count = _pin_count;
        dirty = _dirty;
    }

    friend std::ostream& operator<<(std::ostream& os, const BufferPage& buffer_page) {
        os << "(disk_id, disk_page_cursor, buffer_page_cursor, pin_count, dirty) = (";
        os << buffer_page.disk_id << ", ";
        os << buffer_page.disk_page_cursor << ", ";
        os << buffer_page.buffer_page_cursor << ", ";
        os << buffer_page.pin_count << ", ";
        os << buffer_page.dirty << ")";

        return os;
    }

    DiskId disk_id{0};
    DiskPageCursor disk_page_cursor{0};
    BufferPageCursor buffer_page_cursor{0};
    unsigned int pin_count{0};
    bool dirty{false};
    uint8_t bytes[];
};

#endif // BUFFER_PAGE_H
