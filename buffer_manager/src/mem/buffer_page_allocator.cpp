#include "buffer_page_allocator.h"

#include <cassert>

#include "../buffer_page.h"

BufferPage* BufferPageAllocator::allocate(std::size_t num_buffer_pages) {

}

void BufferPageAllocator::deallocate(BufferPage* buffer_page, std::size_t num_buffer_pages) noexcept {
}


void BufferPageAllocator::init(std::size_t num_pages, std::size_t page_byte_size) {
    if(buffer) {
        m_num_pages = num_pages;
        m_page_byte_size = page_byte_size;

        buffer = static_cast<BufferPage*>(malloc(num_pages * (sizeof(BufferPage) + page_byte_size)));
    }

    assert(0);
}
