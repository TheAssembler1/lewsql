#include "buffer_manager.h"

template<typename T>
const uint8_t& BufferManager<T>::operator[](size_t index) {
  typename DiskManager<T>::Cursor cursor = index / disk_manager.page_size;
  
  // NOTE: check if page is in buffer pool
  if(mem_pool_bit_map[cursor]) {
    return mem_pool[(cursor * disk_manager.page_size)];
  }

  mem_pool_bit_map[cursor] = true;
}
