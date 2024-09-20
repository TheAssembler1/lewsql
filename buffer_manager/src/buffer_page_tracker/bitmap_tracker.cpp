#include "bitmap_tracker.h"

#include <logger.h>

unsigned int BitmapTracker::get_num_free_pages() const {
    return bitmap.get_num_free_bits();
}

unsigned int BitmapTracker::get_num_taken_pages() const {
    return bitmap.get_num_taken_bits();
}

// FIXME: return error
BufferPageCursor BitmapTracker::get_next_free_page() const {
    return bitmap.get_first_free_bit().value();
}

void BitmapTracker::set_page_status(BufferPageCursor buffer_page_cursor, bool val) {
    bitmap.set_bit_val(buffer_page_cursor, val);
}

bool BitmapTracker::get_page_status(BufferPageCursor buffer_page_cursor) const {
    return bitmap.get_bit_val(buffer_page_cursor);
}

std::ostream& operator<<(std::ostream& os, const BitmapTracker& bitmap_tracker) {
    os << bitmap_tracker.bitmap;

    return os;
}
