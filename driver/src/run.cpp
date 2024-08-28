#include "dmanager.h"
#include "posix_dmanager.h"
#include <iostream>

#include <buffer_manager.h>
#include <disk_manager.h>

int main() {
  PosixDManager posix_dmanager("/home/ta1/src/test_dir", 512);
  std::cout << "successfully created disk manager" << std::endl;

  auto disk_id = posix_dmanager.create_disk();

  std::cout << "successfully created disk" << std::endl;
  std::cout << "created disk with disk id: " << disk_id << std::endl;
  
  return 0;
}
