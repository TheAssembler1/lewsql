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

class PosixDiskManager final: public DiskManager<PosixDiskManager> {
  public:
  PosixDiskManager(const std::string& dir_path, unsigned int page_size);
  
  friend std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager);
  
  virtual DiskId create_disk() override;
  virtual void destroy_disk(DiskId disk_id) override;
  virtual void write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) override;
  virtual std::vector<std::byte> read_disk(DiskId disk_id, Cursor cursor) override;

  private:
  std::string get_disk_path(const std::string& f_name) {
    return dir_path + "/" + f_name; }

  std::string get_new_disk_path() {
    return file_prefix + std::to_string(cur_disk);
  }

  // NOTE: returns std::nullopt if filename is not a disk
  static std::optional<unsigned int> get_disk_id_from_path(const std::string& fname) {
    if(fname.size() <= file_prefix.size()) {
      return std::nullopt;
    } else if(fname.substr(0, file_prefix.size()) != file_prefix) {
      return std::nullopt;
    }
    
    std::string disk_id{};
    auto num_suffix = [](auto& c){ try { std::stoi(&c); return true; } catch(std::exception& e) { return false; }};
    std::copy_if(fname.begin(), fname.end(), std::back_inserter(disk_id), num_suffix);

    if(!disk_id.length()) {
      return std::nullopt;
    }

    // FIXME: maybe we throw here... who knows
    return std::stoi(disk_id);
  }
    
  static const inline std::string file_prefix{"posix_dmanager_disk_"};
  unsigned int cur_disk{0};
  // NOTE: maps disk ids to their associated filenames in the dir_path
  std::unordered_map<DiskId, std::string> disk_map{}; 
};

#endif // POSIX_DISK_MANAGER_H
