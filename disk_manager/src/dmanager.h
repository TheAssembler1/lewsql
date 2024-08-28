#ifndef DMANAGER_H
#define DMANAGER_H

#include <variant>
#include <cassert>
#include <vector>
#include <utility>
#include <string>

template<typename T>
class DManager {
  public:

  DManager(const std::string& dir_path, unsigned int page_size) 
    : dir_path{dir_path}, page_size{page_size} {}

  const unsigned int page_size;
  const std::string dir_path;
    
  enum class Error {
    // NOTE: special errors
    NONE = 0,
    NOT_IMPLEMENTED,
    UNKOWN_ERROR,

    // NOTE: specific disk errors
    DISK_ALREADY_EXISTS,
    DISK_NOT_FOUND,

    // NOTE: general disk errors
    CREATE_DISK_ERROR,
    DESTROY_DISK_ERROR,
    WRITE_DISK_ERROR,
    READ_DISK_ERROR,
  };
  
  using DiskId = unsigned int;
  using Cursor = unsigned int;

  virtual std::variant<DiskId, Error> create_disk() = 0;
  virtual Error destroy_disk(DiskId disk_id) = 0;
  virtual Error write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) = 0;
  virtual std::variant<std::vector<std::byte>, Error> read_disk(DiskId disk_id, Cursor cursor) = 0;

};

#endif // DMANAGER_H
