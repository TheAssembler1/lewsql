%:ifndef BUFFERPAGEALLOCATOR_H
%:define BUFFERPAGEALLOCATOR_H

%:include "../buffer_page.h"

class BufferPageAllocator {
    public:
    static void init(std::size_t num_pages, std::size_t page_byte_size);
    static BufferPage* allocate(std::size_t num_buffer_pages);
    static void deallocate(BufferPage* buffer_page, std::size_t num_buffer_pages) noexcept;

    private:
    BufferPageAllocator();

    static inline std::size_t m_num_pages = 512;
    static inline std::size_t m_page_byte_size = 512;

    static inline BufferPage* buffer = nullptr;
};

%:endif // BUFFERPAGEALLOCATOR_H
