#include "posix_dmanager.h"
#include <cstdio>
#include <string>

std::variant<PosixDManager, PosixDManager::Error> PosixDManager::init(const std::string& dir_path, unsigned int page_size) {
  PosixDManager posix_d_manager{dir_path, page_size};
  return posix_d_manager;      
} 

std::variant<PosixDManager::DiskId, PosixDManager::Error> PosixDManager::create_disk() {
  return Error::NOT_IMPLEMENTED;
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

