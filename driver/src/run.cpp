#include "dmanager.h"
#include "posix_dmanager.h"
#include <iostream>

#include <buffer_manager.h>
#include <disk_manager.h>

int main() {
  auto posix_dmanager_variant = PosixDManager::init("/home/ta1/src/test_dir", 512);

  if(std::holds_alternative<PosixDManager::Error>(posix_dmanager_variant)) {
    std::cerr << "failed to create disk manager" << std::endl;
    return 1;
  } else {
    std::cout << "successfully created disk manager" << std::endl;
  }

  auto& posix_dmanager = std::get<PosixDManager>(posix_dmanager_variant);

  auto disk_id_variant = posix_dmanager.create_disk();

  if(std::holds_alternative<PosixDManager::Error>(disk_id_variant)) {
    std::cerr << "failed to create disk" << std::endl;
    return 1;
  } else {
    std::cout << "successfully created disk" << std::endl;
  }

  auto disk_id = std::get<PosixDManager::DiskId>(disk_id_variant);

  std::cout << "created disk with disk id: " << disk_id << std::endl;
  
  return 0;
}
