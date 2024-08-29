#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <variant>
#include <cassert>
#include <vector>
#include <utility>
#include <string>
#include <exception>
#include <memory>

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

  virtual void write_disk(DiskId disk_id, Cursor cursor, uint8_t* bytes) = 0;
  virtual void read_disk(DiskId disk_id, Cursor cursor, uint8_t* bytes) = 0;
};

#endif // DISK_MANAGER_H
