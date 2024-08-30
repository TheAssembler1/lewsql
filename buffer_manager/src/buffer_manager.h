#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "mem_pool.h"
#include "buffer_manager_error.h"

#include <disk_manager.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <vector>
#include <cassert>
#include <cstring>

template<typename T>
using DiskId = typename DiskManager<T>::DiskId;

template<typename T>
using Cursor = typename DiskManager<T>::Cursor;

template<typename T>
class BufferManager final {
  public:
  BufferManager(std::shared_ptr<DiskManager<T>> disk_manager, unsigned int num_pages) 
    : disk_manager{disk_manager}, mem_pool{MemPool(num_pages * disk_manager.page_size)}, mem_pool_bit_map{disk_manager.page_size} {
    assert(num_pages > 0 && disk_manager.page_size > 0);
  }

  void b_write(DiskId<T> disk_id, Cursor<T> cursor, std::unique_ptr<uint8_t[]>, unsigned int num_bytes);
  std::unique_ptr<uint8_t[]> b_read(DiskId<T> disk_id, Cursor<T> cursor, unsigned int num_bytes);

  unsigned int get_next_free_page();

  private:
  unsigned int cur_page{0};
  const std::shared_ptr<DiskManager<T>> disk_manager;

  // FIXME: just using this for development speedfor now
  std::vector<bool> mem_pool_bit_map{};

  // NOTE: maps between disk pages and mem buffer pages
  std::unordered_map<std::pair<DiskId<T>, unsigned int>, unsigned int> mem_pool_map{};

  MemPool mem_pool;
};

#endif // BUFFER_MANAGER_H
