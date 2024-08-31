#ifndef DISK_MANAGER_ERROR
#define DISK_MANAGER_ERROR

#include <exception>
#include <cassert>

enum class DiskManagerErrorCode {
  // NOTE: general error
  UNKOWN_ERROR,

  // NOTE: specific disk errors
  DISK_ALREADY_EXISTS,
  DISK_NOT_FOUND,

  // NOTE: general disk errors
  INIT_DISK_ERROR,
  DEINIT_DISK_ERROR,
  CREATE_DISK_ERROR,
  DESTROY_DISK_ERROR,
  WRITE_DISK_ERROR,
  READ_DISK_ERROR,
};

class DiskManagerError : public std::exception {
  public:
  const DiskManagerErrorCode error_code;
  explicit DiskManagerError(DiskManagerErrorCode error_code) : error_code{error_code} {}
  
  const char* what() const noexcept override {
    switch(error_code) {
        case DiskManagerErrorCode::UNKOWN_ERROR:
            return "disk manager unknown error";
            break;
    case DiskManagerErrorCode::DISK_ALREADY_EXISTS:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::DISK_NOT_FOUND:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::INIT_DISK_ERROR:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::DEINIT_DISK_ERROR:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::CREATE_DISK_ERROR:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::DESTROY_DISK_ERROR:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::WRITE_DISK_ERROR:
        return "disk manager unknown error";
        break;
    case DiskManagerErrorCode::READ_DISK_ERROR:
        return "disk manager unknown error";
        break;
    default:
        assert(1);
        break;
    }
  }
};

#endif // DISK_MANAGER_ERROR