#ifndef BUFFER_MANAGER_ERROR
#define BUFFER_MANAGER_ERROR

#include <exception>

enum class BufferManagerErrorCode {
  UNKOWN_ERROR,
  OUT_OF_MEMORY,
  PAGE_NOT_FOUND,
  DISK_NOT_FOUND,
  OUT_OF_PAGES,
  NO_UNPINNED_PAGES,
};

class BufferManagerError : public std::exception {
  public:
  const BufferManagerErrorCode error_code;
  explicit BufferManagerError(BufferManagerErrorCode error_code) : error_code{error_code} {}
  
  const char* what() const noexcept override {
    return "buffer manager error";
  }
};

#endif // BUFFER_MANAGER_ERROR
