#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <cstdint>
#include <disk_manager.h>
#include <iostream>
#include <logger.h>

#include "buffer_manager_error.h"
#include "buffer_manager_types.h"

class BufferPage {
    public:
    BufferPage() = delete;
    ~BufferPage() = delete;
    BufferPage(const BufferPage&) = delete;
    BufferPage& operator=(const BufferPage&) = delete;

    void init(unsigned int _page_size,
    DiskId _disk_id,
    DiskPageCursor _disk_page_cursor,
    BufferPageCursor _buffer_page_cursor,
    unsigned int _pin_count,
    bool _dirty) {
        page_size = _page_size;
        disk_id = _disk_id;
        disk_page_cursor = _disk_page_cursor;
        buffer_page_cursor = _buffer_page_cursor;
        pin_count = _pin_count;
        dirty = _dirty;

        initialized = true;
    }

    template <typename T> T* to_ptr(unsigned int offset) {
        assert(initialized);

        if(offset >= page_size) {
            LOG(LogLevel::ERROR) << "invalid page offset: " << offset << std::endl;
            throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGE_BOUNDS);
        }

        return reinterpret_cast<T*>(&bytes[offset]);
    }

    template <typename T> void memcpy(unsigned int offset, const T* elements, unsigned int num_elements) {
        assert(initialized);

        if(offset + (sizeof(T) * num_elements) > page_size) {
            LOG(LogLevel::ERROR) << "starting offset: " << offset << std::endl;
            LOG(LogLevel::ERROR)
            << "invalid offset + (sizeof(T) * num_elements): " << offset + (sizeof(T) * num_elements) << std::endl;
            throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGE_BOUNDS);
        }

        for(int i = 0; i < num_elements; i++) {
            unsigned int final_offset = offset + (i * sizeof(T));
            *to_ptr<T>(final_offset) = elements[i];
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const BufferPage& buffer_page) {
        assert(buffer_page.initialized);

        os << "(disk_id, disk_page_cursor, buffer_page_cursor, pin_count, dirty) = (";
        os << buffer_page.disk_id << ", ";
        os << buffer_page.disk_page_cursor << ", ";
        os << buffer_page.buffer_page_cursor << ", ";
        os << buffer_page.pin_count << ", ";
        os << buffer_page.dirty << ")";

        return os;
    }

    static unsigned int size(unsigned int page_size) {
        return sizeof(BufferPage) + page_size;
    }

    DiskId disk_id{0};
    DiskPageCursor disk_page_cursor{0};
    BufferPageCursor buffer_page_cursor{0};

    unsigned int pin_count{0};
    unsigned int page_size{0};

    bool dirty{false};
    bool initialized{false};

    uint8_t bytes[];
};

#endif // BUFFER_PAGE_H
