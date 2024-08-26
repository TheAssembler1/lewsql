#include "posix_dmanager.h"
#include <cstdio>
#include <string>

std::variant<PosixDManager, PosixDManager::Error> PosixDManager::init(const std::string& dir_path, unsigned int page_size) {
    PosixDManager posix_d_manager{dir_path, page_size};
    return posix_d_manager;      
} 

std::variant<PosixDManager::DiskId, PosixDManager::Error> PosixDManager::create_disk() {
  std::string path = get_disk_path(file_prefix + std::to_string(cur_disk));
  int st = access(path.c_str(), R_OK | W_OK);

  // FIXME: proper error return here
  if(st == -1) {
    FILE* fd = fopen(path.c_str(), "w");
    disk_map[cur_disk] = path.c_str();
    fclose(fd);
    cur_disk++;
  }

  // FIXME: assumes file doesn't already exists
  assert(0);
}

PosixDManager::Error PosixDManager::destroy_disk(DiskId disk_id) {
  
}

PosixDManager::Error PosixDManager::write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) {
  assert(bytes.size() == page_size);
  assert(disk_map.count(disk_id) != 0);
  
  // NOTE: grab current file offset in disk file
  int f_offset = cursor * page_size;

  FILE* fd = popen(disk_map[disk_id].c_str(), "w");

  int stat = fseek(fd, f_offset, SEEK_SET);
  assert(stat != -1);

  stat = fwrite(bytes.data(), page_size, 1, fd);

  fclose(fd);

  return Error::NONE;
}

std::variant<std::vector<std::byte>, PosixDManager::Error> PosixDManager::read_disk(DiskId disk_id, Cursor cursor) {
  
}

