#include "buffer_manager.h"

template<typename T>
unsigned int BufferManager<T>::get_next_free_page() {
  int cur_page = 0;

  while(!mem_pool_bit_map[cur_page]) {
    cur_page++;

    if(cur_page == mem_pool_bit_map.size()) {
      throw BufferManagerError(BufferManagerErrorCode::OUT_OF_MEMORY);
    }
  }

  return cur_page;
}

template<typename T>
void BufferManager<T>::b_write(DiskId<T> disk_id, Cursor<T> cursor, std::unique_ptr<uint8_t[]>, unsigned int num_bytes) {
  Cursor<T> init_cursor = cursor;
  unsigned int cur_byte = 0;
  
  while(cur_byte < num_bytes) {
    unsigned int page_index = cur_byte / disk_manager.page_size;
    unsigned int page_offset = cur_byte % disk_manager.page_size;
    unsigned int remaining_bytes = num_bytes - cur_byte;
    unsigned int remaining_page_bytes = disk_manager.page_size - page_offset;

    unsigned num_bytes_to_write = std::max(remaining_bytes, remaining_page_bytes);

    std::unique_ptr<uint8_t[]> page;
    auto mem_pool_map_iter = mem_pool_map.find(std::make_pair(disk_id, page_index));

    unsigned int mem_pool_page_index;
    if(mem_pool_map_iter == mem_pool_map.end()) {
      auto new_page_index = get_next_free_page();
      uint8_t* dest_ptr = &mem_pool.pool[new_page_index * disk_manager.page_size];
      disk_manager.d_read(disk_id, page_index, dest_ptr);
    } else {
      mem_pool_page_index = mem_pool_map_iter->second;      
    }

  }
  
}

template<typename T>
std::unique_ptr<uint8_t[]> BufferManager<T>::b_read(DiskId<T> disk_id, Cursor<T> cursor, unsigned int num_bytes) {
  
}
