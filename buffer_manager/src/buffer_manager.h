#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "mem_pool.h"

#include <disk_manager.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <vector>
#include <cassert>

// NOTE: responsible for holding cursor state for allocated pages
template<typename T>
struct BufferDiskHandler {
  typename DiskManager<T>::DiskId disk_id;
  typename DiskManager<T>::Cursor cursor;
};

template<typename T>
class BufferManager final {
  public:
  BufferManager(std::shared_ptr<DiskManager<T>> disk_manager, unsigned int num_pages) 
    : disk_manager{disk_manager}, mem_pool{MemPool(num_pages * disk_manager.page_size)}, mem_pool_bit_map{disk_manager.page_size} {
    assert(num_pages > 0 && disk_manager.page_size > 0);
  }

  void set_buffer_disk_handler(typename DiskManager<T>::DiskId disk_id, typename DiskManager<T>::Cursor cursor) {
    buffer_disk_handler.disk_id = disk_id;
    buffer_disk_handler.cursor = cursor;
  }
  void set_buffer_disk_handler_cursor(typename DiskManager<T>::Cursor cursor) { disk_manager.cursor = cursor; };
  void set_buffer_disk_handler_disk_id(typename DiskManager<T>::DiskId disk_id) { disk_manager.disk_id = disk_id; };

  const uint8_t& operator[](size_t index);

  private:
  BufferDiskHandler<T> buffer_disk_handler{};
  unsigned int cur_page{0};
  const std::shared_ptr<DiskManager<T>> disk_manager;
  // FIXME: just using this for development speedfor now
  std::vector<bool> mem_pool_bit_map;
  MemPool mem_pool;
};

#endif // BUFFER_MANAGER_H
