#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <variant>
#include <cassert>
#include <vector>
#include <utility>
#include <string>
#include <exception>
#include <memory>

#include "disk_manager_types.h"

class DiskManager {
  public:
  DiskManager(const std::string& dir_path, unsigned int page_size) 
    : page_size{page_size}, dir_path{dir_path} {}

  const unsigned int page_size;
  const std::string dir_path;

  virtual DiskId d_create() = 0;
  virtual void d_destroy(DiskId disk_id) = 0;

  virtual void d_write(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) = 0;
  virtual void d_read(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) = 0;
};

#endif // DISK_MANAGER_H
