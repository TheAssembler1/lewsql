#ifndef POSIX_DISK_MANAGER_H
#define POSIX_DISK_MANAGER_H

#include <iterator>
#ifdef __unix__
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <cerrno>
#else
#error "POSIX compliance required"
#endif 

#include <iostream>
#include <string>
#include <disk_manager.h>
#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <optional>
#include <cstdlib>

class PosixDiskManager final: public DiskManager {
  public:
  PosixDiskManager(const std::string& dir_path);
  
  friend std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager);
  
  virtual DiskId d_create(DiskName disk_name) override;
  virtual void d_destroy(DiskId disk_id) override;

  virtual void d_write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes, unsigned int page_size) override;
  virtual void d_read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes, unsigned int page_size) override;

  virtual DiskName disk_exists(DiskId disk_id) override;
  virtual unsigned int disk_size(DiskId disk_id) override;

  private:
  std::string get_disk_path(const DiskName& disk_name) { return dir_path + "/" + disk_name; }
  DiskId cur_disk_id = 0;

  std::unordered_map<DiskId, DiskName> disks{};
};

#endif // POSIX_DISK_MANAGER_H
