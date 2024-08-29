#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <variant>
#include <cassert>
#include <vector>
#include <utility>
#include <string>
#include <exception>
#include <memory>

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
    return "disk manager error";
  }
};

template<typename T>
class DiskManager {
  public:

  DiskManager(const std::string& dir_path, unsigned int page_size) 
    : dir_path{dir_path}, page_size{page_size} {}

  const unsigned int page_size;
  const std::string dir_path;
  
  using DiskId = unsigned int;
  using Cursor = unsigned int;

  virtual DiskId create_disk() = 0;
  virtual void destroy_disk(DiskId disk_id) = 0;

  virtual void write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) = 0;
  virtual std::vector<std::byte> read_disk(DiskId disk_id, Cursor cursor) = 0;
};

#endif // DISK_MANAGER_H
