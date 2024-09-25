%:ifndef BUFFERPAGEALLOCATOR_H
%:define BUFFERPAGEALLOCATOR_H

%:include "../buffer_page.h"

class BufferPageAllocator {
    public:
    BufferPageAllocator();

    BufferPage* allocate(std::size_t num_pages);
    void deallocate(BufferPage* buffer_page, std::size_t n) noexcept;
};

%:endif // BUFFERPAGEALLOCATOR_H
