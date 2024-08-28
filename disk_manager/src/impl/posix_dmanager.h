#ifndef POSIX_DMANAGER_H
#define POSIX_DMANAGER_H

#include <iterator>
#ifdef __unix__
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#else
#error "POSIX compliance required"
#endif 

#include <iostream>
#include <string>
#include <dmanager.h>
#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <optional>
#include <cstdlib>

class PosixDManager : public DiskManager<PosixDManager> {
  public:
  PosixDManager(const std::string& dir_path, unsigned int page_size) : DiskManager{dir_path, page_size} {
    std::cout << "dir path set to path: " << dir_path << std::endl;
    std::cout << "disk manager page size set to size: " << page_size << std::endl;

    // NOTE: get all files in dir and if valid filename add to disk map
    errno = 0;
    DIR* dir = opendir(dir_path.c_str());
    struct dirent* ent = nullptr;

    if(!dir) {
      std::cerr << "failed to open directory, errno: " << errno << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    std::vector<std::string> ent_names{};
    while((ent = readdir(dir)) != nullptr) {
      std::string fname{ent->d_name};
      std::optional<unsigned int> disk_id_opt = get_disk_id_from_path(fname);

      if(disk_id_opt.has_value()) {
        unsigned int disk_id = disk_id_opt.value();

        std::cout << "found disk with id: " << disk_id << std::endl;

        // NOTE: update to latest disk id in the disk manager path
        if(cur_disk <= disk_id) {
          cur_disk = disk_id + 1;
        }
        
        disk_map[disk_id] = fname;
        ent_names.emplace_back(fname);
      } else {
        std::cout << "found invalid disk with filename: " << fname << std::endl;
      }
    }

    std::cout << "total number of possible path entires: " << ent_names.size() << std::endl;
    std::cout << "current disk id: " << cur_disk << std::endl;

    errno = 0;
    int stat = closedir(dir);

    if(stat == -1) {
      std::cerr << "failed to close directory, errno: " << errno << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }
  }
  
  virtual DiskId create_disk() override;
  virtual void destroy_disk(DiskId disk_id) override;
  virtual void write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) override;
  virtual std::vector<std::byte> read_disk(DiskId disk_id, Cursor cursor) override;

  virtual ~PosixDManager() = default;
  private:
  std::string get_disk_path(const std::string& f_name) {
    std::cout << "creating file for disk with name: " << f_name << std::endl;
    return dir_path + "/" + f_name;
  }

  std::string get_new_disk_path() {
    return get_disk_path(file_prefix + std::to_string(cur_disk));
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

#endif // POSIX_DMANAGER_H
