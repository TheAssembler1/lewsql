#include "posix_disk_manager.h"
#include <fcntl.h>

PosixDiskManager::PosixDiskManager(const std::string& dir_path, unsigned int page_size) : DiskManager{dir_path, page_size} {
  std::cout << "dir path set to path: " << dir_path << std::endl;
  std::cout << "disk manager page size set to size: " << page_size << std::endl;

  // NOTE: get all files in dir and if valid filename add to disk map
  errno = 0;
  DIR* dir = opendir(dir_path.c_str());
  struct dirent* ent = nullptr;

  if(!dir) {
    std::cerr << "failed to open directory, errno: " << std::strerror(errno) << std::endl;
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
}

PosixDiskManager::DiskId PosixDiskManager::create_disk() {
  std::string fname = get_new_disk_path();
  std::string full_path_fname = get_disk_path(fname);

  // NOTE: check if file already exists
  errno = 0;
  int st = access(full_path_fname.c_str(), F_OK);

  // NOTE: access fails ensure it was due to it not existing
  if(st == -1) {
    if(errno != ENOENT) {
      std::cerr << "failed to access file, errno: " << std::strerror(errno) << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }
  } else { // NOTE: file already exists
    std::cerr << "file already exists" << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
  }

  errno = 0;
  int fd = open(full_path_fname.c_str(), O_CREAT, 0644);

  // FIXME: return more specific errors based on errno status
  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
  } else {
    std::cout << "created file at path: " << fname << std::endl;
  }

  errno = 0;
  st = close(fd);

  if(st == -1) {
    std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
  }

  // NOTE: copy cur disk id for return and inc
  DiskId ret = cur_disk;  

  // NOTE: adding disk to disk map
  disk_map[ret] = fname;

  cur_disk++;
  return ret;
}

void PosixDiskManager::destroy_disk(DiskId disk_id) {
  auto fname_iter = disk_map.find(disk_id);

  if(fname_iter == disk_map.end()) {
    throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
  }

  auto fname = fname_iter->second;
  auto full_path_fname =get_disk_path(fname);

  errno = 0;
  int stat = unlink(full_path_fname.c_str());

  if(stat == -1) {
    std::cerr << "failed to unlink file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
  }
  
  disk_map.erase(disk_id);
}

void PosixDiskManager::write_disk(DiskId disk_id, Cursor cursor, std::vector<std::byte>& bytes) {
  assert(bytes.size() == page_size);

  auto fname = disk_map.find(disk_id);

  if(fname == disk_map.end()) {
    throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
  }

  errno = 0;
  std::string abs_path = get_disk_path(fname->second);
  int fd = open(abs_path.c_str(), O_WRONLY);

  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << std::strerror(errno) << " file path: " << abs_path << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  // FIXME: validation on this number
  int foffset = cursor * page_size;

  errno = 0;
  int stat = lseek(fd, foffset, SEEK_SET);

  if(stat == -1) {
    std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  int num_bytes = write(fd, bytes.data(), page_size);

  if(num_bytes != page_size) {
      std::cerr << "failed to write num_bytes to file, errno: " << std::strerror(errno) << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  stat = close(fd);

  if(stat == -1) {
    std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }
}

std::vector<std::byte> PosixDiskManager::read_disk(DiskId disk_id, Cursor cursor) {
  auto fname_iter = disk_map.find(disk_id);

  if(fname_iter == disk_map.end()) {
    throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
  }

  auto fname = fname_iter->second;
  auto full_path_fname = get_disk_path(fname);

  errno = 0;
  int fd = open(full_path_fname.c_str(), O_RDONLY);

  if(fd == -1) {
    std::cerr << "failed to open file, errno: " << std::strerror(errno) << "file path: " << full_path_fname << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  // FIXME: validation on this number
  int foffset = ((cursor + 1) * page_size);

  errno = 0;
  int stat = lseek(fd, foffset, SEEK_SET);
  if(stat == -1) {
    std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  // NOTE: reset file pointer
  errno = 0;
  stat = lseek(fd, foffset - page_size, SEEK_SET);
  if(stat == -1) {
    std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  std::vector<std::byte> bytes(page_size);

  std::cout << "offset before read: " << foffset << std::endl;
  int num_bytes = read(fd, bytes.data(), page_size);

  if(num_bytes != page_size) {
      std::cerr << "failed to read num_bytes to file, errno: " << std::strerror(errno) << " num bytes: " << num_bytes << std::endl;
      throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  errno = 0;
  stat = close(fd);

  if(stat == -1) {
    std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
    throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
  }

  return bytes;
}

std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager) {
  int cur_size = 1;
  for(const auto& [key, val]: posix_disk_manager.disk_map) {
    std::cout << "[" << key << ", " << val << "]";

    if(cur_size != posix_disk_manager.disk_map.size()) {
      std::cout << std::endl;
    }
    
    cur_size++;
  }
  
  return os;
}
