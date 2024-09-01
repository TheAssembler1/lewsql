#include "bitmap_tracker.h"

unsigned int BitmapTracker::get_num_free_pages() const {
    int total_free = 0;
    for(auto page: bitmap) {
        if(!page) {
            total_free++;
        }
    }

    return total_free;
}

unsigned int BitmapTracker::get_num_taken_pages() const {
    return bitmap.size() - get_num_free_pages();
}


BufferPageCursor BitmapTracker::get_next_free_page() const {
    std::cout << "getting next free page" << std::endl;

    BufferPageCursor cursor = 0;

    while(bitmap[cursor]) {
        cursor++;
    }

    if(cursor == bitmap.size()) {
        throw BufferManagerError(BufferManagerErrorCode::OUT_OF_PAGES);
    }

    return cursor;
}

void BitmapTracker::set_page_status(BufferPageCursor buffer_page_cursor, bool taken) {
    bitmap[buffer_page_cursor] = taken;
}

bool BitmapTracker::get_page_status(BufferPageCursor buffer_page_cursor) const {
    return bitmap[buffer_page_cursor];
}

std::ostream& operator<<(std::ostream& os, const BitmapTracker& bitmap_tracker) {
    unsigned int index = 0;
    while(index < bitmap_tracker.bitmap.size()) {
        if(bitmap_tracker.bitmap[index]) {
            os << "1";
        } else {
            os << "0";
        }
        index++;
        if(index == bitmap_tracker.bitmap.size()) {
            break;
        }
        if(index && index % 32 == 0) {
            os << std::endl;
        } else {
            os << ":";
        }
    }

    return os;
}
