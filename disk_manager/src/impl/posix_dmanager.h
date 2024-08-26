#ifndef POSIX_DMANAGER_H
#define POSIX_DMANAGER_H

#ifdef __unix__
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#else
#error "POSIX compliance required"
#endif 

#include <string>
#include <dmanager.h>
#include <unordered_map>

class PosixDManager : public DManager<PosixDManager> {
  private:
  PosixDManager(const std::string& dir_path, unsigned int page_size) : DManager{dir_path, page_size} {}
  
  public:
  std::variant<PosixDManager, Error> init(const std::string& dir_path, unsigned int page_size) override;

  std::variant<DiskId, Error> create_disk() override;
  Error destroy_disk(DiskId disk_id) override;
  Error write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) override;
  std::variant<std::vector<std::byte>, Error> read_disk(DiskId disk_id, Cursor cursor) override;

  virtual ~PosixDManager() = default;
  private:
  std::string get_disk_path(const std::string& f_name) {
    return dir_path + f_name;
  }
    
  static const inline std::string file_prefix{"posix_dmanager_disk_"};
  unsigned int cur_disk{0};
  // NOTE: maps disk ids to their associated filenames in the dir_path
  std::unordered_map<DiskId, std::string> disk_map{}; 
};

#endif // POSIX_DMANAGER_H
