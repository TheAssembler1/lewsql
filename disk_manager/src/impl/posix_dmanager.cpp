#include "posix_dmanager.h"

std::variant<PosixDManager, PosixDManager::Error> PosixDManager::init(const std::string& dir_path, unsigned int page_size) {
  PosixDManager posix_d_manager{dir_path, page_size};
  return posix_d_manager;      
} 
 
std::variant<PosixDManager::DiskId, PosixDManager::Error> PosixDManager::create_disk() {
  std::string fname = get_new_disk_path();

  // NOTE: check if file already exists
  errno = 0;
  int st = access(fname.c_str(), F_OK);

  // NOTE: access fails ensure it was due to it not existing
  if(st == -1) {
    if(errno != ENOENT) {
      std::cerr << "failed to access file, errno: " << errno << std::endl;
      return Error::CREATE_DISK_ERROR;
    }
  } else { // NOTE: file already exists
    std::cerr << "file already exists" << std::endl;
    return Error::DISK_ALREADY_EXISTS;
  }

  errno = 0;
  int fd = open(fname.c_str(), O_CREAT);

  // FIXME: return more specific errors based on errno status
  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << errno << std::endl;
    return Error::CREATE_DISK_ERROR;
  } else {
    std::cout << "created file at path: " << fname << std::endl;
  }

  errno = 0;
  st = close(fd);

  if(st == -1) {
    std::cerr << "failed to close file, errno: " << errno << std::endl;
    return Error::CREATE_DISK_ERROR;
  }

  // NOTE: copy cur disk id for return and inc
  DiskId ret = cur_disk;  
  cur_disk++;

  return ret;
}

PosixDManager::Error PosixDManager::destroy_disk(DiskId disk_id) {
  return Error::NOT_IMPLEMENTED;
}

PosixDManager::Error PosixDManager::write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) {
  return Error::NOT_IMPLEMENTED;
}

std::variant<std::vector<std::byte>, PosixDManager::Error> PosixDManager::read_disk(DiskId disk_id, Cursor cursor) {
  return Error::NOT_IMPLEMENTED;
}

