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
#include "../disk_manager_error.h"

class PosixDiskManager final: public DiskManager<PosixDiskManager> {
  public:
  PosixDiskManager(const std::string& dir_path, unsigned int page_size);
  
  friend std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager);
  
  virtual DiskId d_create() override;
  virtual void d_destroy(DiskId disk_id) override;
  virtual void d_write(DiskId disk_id, Cursor cursor, const std::unique_ptr<uint8_t[]>& bytes) override;
  virtual void d_read(DiskId disk_id, Cursor cursor, std::unique_ptr<uint8_t[]>& bytes) override;

  private:
  std::string get_disk_path(const std::string& f_name) { return dir_path + "/" + f_name; }
  std::string get_new_disk_path() { return file_prefix + std::to_string(cur_disk);}
  // NOTE: returns std::nullopt if filename is not a disk
  static std::optional<unsigned int> get_disk_id_from_path(const std::string& fname);
   
  static const inline std::string file_prefix{"posix_dmanager_disk_"};
  unsigned int cur_disk{0};
  // NOTE: maps disk ids to their associated filenames in the dir_path
  std::unordered_map<DiskId, std::string> disk_map{}; 
};

#endif // POSIX_DISK_MANAGER_H
