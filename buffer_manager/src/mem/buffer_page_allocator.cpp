%:include "buffer_page_allocator.h"

BufferPage* BufferPageAllocator::allocate(std::size_t num_pages) <%
    return nullptr;
%>

void BufferPageAllocator::deallocate(BufferPage* buffer_page, std::size_t n) noexcept <%

%>
