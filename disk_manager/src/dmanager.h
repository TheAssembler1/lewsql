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
  const std::string& dir_path;
    
  enum class Error {
    NONE = 0,
    UNKOWN_ERROR,
    DISK_ALREADY_EXISTS,
    DISK_NOT_FOUND,
  };
  
  using DiskId = unsigned int;
  using Cursor = unsigned int;

  virtual std::variant<T, Error> init(const std::string& dir_path, unsigned int page_size);
  virtual std::variant<DiskId, Error> create_disk() = 0;
  virtual Error destroy_disk(DiskId disk_id) = 0;
  virtual Error write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) = 0;
  virtual std::variant<std::vector<std::byte>, Error> read_disk(DiskId disk_id, Cursor cursor) = 0;

};

#endif // DMANAGER_H
