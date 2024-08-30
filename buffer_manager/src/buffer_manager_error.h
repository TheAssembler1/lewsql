#ifndef BUFFER_MANAGER_ERROR
#define BUFFER_MANAGER_ERROR

#include <exception>

enum class BufferManagerErrorCode {
  // NOTE: general error
  UNKOWN_ERROR,

  OUT_OF_MEMORY,
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
