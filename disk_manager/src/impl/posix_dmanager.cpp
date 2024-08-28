#include "posix_dmanager.h"
#include "dmanager.h"
#include <fcntl.h>

PosixDManager::DiskId PosixDManager::create_disk() {
  std::string fname = get_new_disk_path();

  // NOTE: check if file already exists
  errno = 0;
  int st = access(fname.c_str(), F_OK);

  // NOTE: access fails ensure it was due to it not existing
  if(st == -1) {
    if(errno != ENOENT) {
      std::cerr << "failed to access file, errno: " << errno << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }
  } else { // NOTE: file already exists
    std::cerr << "file already exists" << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
  }

  errno = 0;
  int fd = open(fname.c_str(), O_CREAT);

  // FIXME: return more specific errors based on errno status
  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
  } else {
    std::cout << "created file at path: " << fname << std::endl;
  }

  errno = 0;
  st = close(fd);

  if(st == -1) {
    std::cerr << "failed to close file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
  }

  // NOTE: copy cur disk id for return and inc
  DiskId ret = cur_disk;  

  // NOTE: adding disk to disk map
  disk_map[ret] = fname;

  cur_disk++;
  return ret;
}

void PosixDManager::destroy_disk(DiskId disk_id) {
  auto fname_iter = disk_map.find(disk_id);

  if(fname_iter == disk_map.end()) {
    throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
  }

  errno = 0;
  int stat = unlink(fname_iter->second.c_str());

  if(stat == -1) {
    std::cerr << "failed to unlink file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
  }
  
  disk_map.erase(disk_id);
}

void PosixDManager::write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) {
  assert(bytes.size() == page_size);

  auto fname = disk_map.find(disk_id);

  if(fname == disk_map.end()) {
    throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
  }

  errno = 0;
  int fd = open(get_disk_path(fname->second).c_str(), O_WRONLY);

  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  // FIXME: validation on this number
  int foffset = cursor * page_size;

  errno = 0;
  int stat = lseek(fd, foffset, SEEK_SET);

  if(stat == -1) {
    std::cerr << "failed to lseek file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  int num_bytes = write(fd, bytes.data(), page_size);

  if(num_bytes != page_size) {
      std::cerr << "failed to write num_bytes to file, errno: " << errno << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  stat = close(fd);

  if(stat == -1) {
    std::cerr << "failed to close file, errno: " << errno << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }
}

std::vector<std::byte> PosixDManager::read_disk(DiskId disk_id, Cursor cursor) {
  return std::vector<std::byte>{};
}

