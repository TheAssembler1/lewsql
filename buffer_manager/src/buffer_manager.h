#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "disk_manager.h"
#include "mem_pool.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <vector>

template<typename T>
class BufferManager final {
  public:
  BufferManager(unsigned int num_pages, unsigned int page_size) : mem_pool{MemPool(num_pages * page_size)} {
    assert(num_pages > 0 && page_size > 0);
  }

  private:
  unsigned int cur_page{0};
  std::priority_queue<unsigned int> buffer_queue;
  const std::shared_ptr<DiskManager<T>> disk_manager;
  MemPool mem_pool;
};

#endif // BUFFER_MANAGER_H
