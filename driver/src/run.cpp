#include "dmanager.h"
#include "posix_dmanager.h"
#include <iostream>

#include <buffer_manager.h>
#include <disk_manager.h>
#include <vector>

int main() {
  PosixDManager posix_dmanager("/home/ta1/src/test_dir", 512);
  std::cout << "successfully created disk manager" << std::endl;

  auto disk_id = posix_dmanager.create_disk();

  std::vector<std::byte> bytes(512);
  posix_dmanager.write_disk(disk_id, 0, bytes);

  std::cout << "successfully created disk" << std::endl;
  std::cout << "created disk with disk id: " << disk_id << std::endl;
  
  return 0;
}
