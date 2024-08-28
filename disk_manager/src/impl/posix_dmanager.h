#ifndef POSIX_DMANAGER_H
#define POSIX_DMANAGER_H

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

class PosixDManager : public DManager<PosixDManager> {
  private:
  PosixDManager(const std::string& dir_path, unsigned int page_size) : DManager{dir_path, page_size} {
    std::cout << "dir path set to path: " << dir_path << std::endl;
    std::cout << "disk manager page size set to size: " << page_size << std::endl;
  }
  
  public:
  static std::variant<PosixDManager, Error> init(const std::string& dir_path, unsigned int page_size);

  std::variant<DiskId, Error> create_disk() override;
  Error destroy_disk(DiskId disk_id) override;
  Error write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) override;
  std::variant<std::vector<std::byte>, Error> read_disk(DiskId disk_id, Cursor cursor) override;

  virtual ~PosixDManager() = default;
  private:
  std::string get_disk_path(const std::string& f_name) {
    std::cout << "creating file for disk with name: " << f_name << std::endl;
    return dir_path + "/" + f_name;
  }

  std::string get_new_disk_path() {
    return get_disk_path(file_prefix + std::to_string(cur_disk));
  }
    
  static const inline std::string file_prefix{"posix_dmanager_disk_"};
  unsigned int cur_disk{0};
  // NOTE: maps disk ids to their associated filenames in the dir_path
  std::unordered_map<DiskId, std::string> disk_map{}; 
};

#endif // POSIX_DMANAGER_H
