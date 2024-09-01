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
  DiskManager(const std::string& dir_path) : dir_path{dir_path} {}

  const std::string dir_path;

  virtual DiskId d_create(const DiskName& disk_name) = 0;
  virtual void d_destroy(DiskId disk_id) = 0;

  virtual DiskId d_load(const DiskName& disk_name) = 0;
  virtual void d_unload(DiskId disk_id) = 0;

  virtual void d_write(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes, unsigned int page_size) = 0;
  virtual void d_read(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes, unsigned int page_size) = 0;

  virtual DiskName disk_loaded(DiskId disk_id) = 0;
  virtual unsigned int disk_size(DiskId disk_id) = 0;
};

#endif // DISK_MANAGER_H
