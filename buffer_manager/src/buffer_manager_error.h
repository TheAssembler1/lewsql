#ifndef BUFFER_MANAGER_ERROR
#define BUFFER_MANAGER_ERROR

#include <exception>
#include <cassert>

enum class BufferManagerErrorCode {
  UNKOWN_ERROR,
  OUT_OF_MEMORY,
  PAGE_NOT_FOUND,
  DISK_NOT_FOUND,
  DISK_PAGE_CURSOR_NOT_FOUND,
  OUT_OF_PAGES,
};

class BufferManagerError : public std::exception {
  public:
  const BufferManagerErrorCode error_code;
  explicit BufferManagerError(BufferManagerErrorCode error_code) : error_code{error_code} {}

  const char* what() const noexcept override {
    switch(error_code) {
        case BufferManagerErrorCode::UNKOWN_ERROR:
            return "buffer manager unknown error";
        break;
        case BufferManagerErrorCode::OUT_OF_MEMORY:
            return "buffer manager out of memory";
        break;
        case BufferManagerErrorCode::PAGE_NOT_FOUND:
            return "buffer manager page not found";
        break;
        case BufferManagerErrorCode::DISK_NOT_FOUND:
            return "buffer manager disk not found";
        break;
        case BufferManagerErrorCode::DISK_PAGE_CURSOR_NOT_FOUND:
            return "buffer manager disk page cursor not found";
        break;
        case BufferManagerErrorCode::OUT_OF_PAGES:
            return "buffer manager out of pages";
        break;
        default:
            assert(0);
            break;
    }
  }
};

#endif // BUFFER_MANAGER_ERROR
