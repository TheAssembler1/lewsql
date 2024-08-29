#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <iostream>

#include <disk_manager.h>
#include <vector>

int main() {
  PosixDiskManager posix_dmanager("/home/ta1/src/test_dir", 512);
  std::cout << "successfully created disk manager" << std::endl;

  std::cout << posix_dmanager << std::endl;

  auto disk_id = posix_dmanager.create_disk();
  std::cout << "successfully created disk" << std::endl;

  std::vector<std::byte> bytes(512);
  bytes[0] = std::byte{1};
  bytes[1] = std::byte{2};
  bytes[2] = std::byte{3};
  bytes[3] = std::byte{4};
  bytes[4] = std::byte{5};
  bytes[5] = std::byte{6};
  bytes[6] = std::byte{7};

  posix_dmanager.write_disk(disk_id, 0, bytes);
  std::cout << "successfully wrote to disk" << std::endl;

  std::vector<std::byte> read_bytes = posix_dmanager.read_disk(disk_id, 0);
  std::cout << "successfully read from disk" << std::endl;

  std::cout << "size of read bytes: " << read_bytes.size() << std::endl;
  int cur  = 0;
  for(auto& byte: read_bytes) {
    std::cout << static_cast<unsigned int>(byte) << std::endl;
    cur++;
    if(cur > 8) {
      break;
    }
  }

  posix_dmanager.destroy_disk(disk_id);
  std::cout << "successfully destroyed disk" << std::endl;
  
  return 0;
}
