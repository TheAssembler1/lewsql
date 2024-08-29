#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "disk_manager.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <vector>

static void bytes_deleter(uint8_t* bytes) {
  delete[] bytes;
}

template<typename T>
class BufferManager final {
  BufferManager(std::shared_ptr<DiskManager<T>> disk_manager): disk_manager{disk_manager} {
      buffer_pool = std::vector<std::vector<std::byte>>{512};
  }

  std::unique_ptr<uint8_t[], decltype(&bytes_deleter)> read_disk(typename DiskManager<T>::Cursor cursor, unsigned int num_bytes);


  // NOTE: acts as a ordering stamp
  unsigned int cur_page = 0;
  std::vector<std::vector<std::byte>> buffer_pool;
  std::priority_queue<unsigned int> buffer_queue;
  

  const std::shared_ptr<DiskManager<T>> disk_manager;
};

#endif // BUFFER_MANAGER_H
