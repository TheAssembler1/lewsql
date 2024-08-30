#include <disk_manager.h>
#include <buffer_manager.h>
#include <impl/posix_disk_manager.h>
#include <iostream>

#include <disk_manager.h>
#include <vector>

int main() {
  PosixDiskManager posix_dmanager("/home/ta1/src/test_dir", 512);
  std::cout << "successfully created disk manager" << std::endl;

  std::cout << posix_dmanager << std::endl;

  auto disk_id = posix_dmanager.d_create();
  std::cout << "successfully created disk" << std::endl;

  std::unique_ptr<uint8_t[]> bytes{new uint8_t[512]};
  for(int i = 0; i < 8; i++) {
    bytes[i] = i;
    std::cout << "write byte: " <<  static_cast<unsigned int>(bytes[i]) << std::endl;
  }
  posix_dmanager.d_write(disk_id, 0, bytes);
  std::cout << "successfully wrote to disk" << std::endl;

  std::unique_ptr<uint8_t[]> read_bytes{new uint8_t[512]};
  posix_dmanager.d_read(disk_id, 0, read_bytes);
  std::cout << "successfully read from disk" << std::endl;

  for(int i = 0; i < 8; i++) {
    std::cout << "read byte: " <<  static_cast<unsigned int>(read_bytes[i]) << std::endl;
  }

  // NOTE: read from disk manager into memory buffer
  BufferManager<PosixDiskManager> buffer_manager{&posix_dmanager, 512, };

  posix_dmanager.d_destroy(disk_id);
  std::cout << "successfully destroyed disk" << std::endl;

  return 0;
}
