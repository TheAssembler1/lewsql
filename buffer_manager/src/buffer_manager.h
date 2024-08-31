#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <disk_manager.h>
#include <disk_manager_types.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>

#include "buffer_manager_error.h"
#include "replacement/replacement_alg.h"
#include "buffer_manager_types.h"

class BufferManager final {
  public:
  BufferManager(std::shared_ptr<DiskManager> disk_manager,
                std::shared_ptr<ReplacementAlg> replacement_alg,
                unsigned int num_pages)
    : disk_manager{disk_manager},
      replacement_alg{replacement_alg},
      mem_pool_bitmap(num_pages),
      mem_pool{new uint8_t[num_pages * disk_manager->page_size]}{
    assert(num_pages > 0 && disk_manager->page_size > 0);

    std::cout << "creating buffer manager with (num_pages, page_size) = ("
              << num_pages << ", " << disk_manager->page_size << ")" << std::endl;
  }

  uint8_t* pin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void unpin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor);

  static std::ostream& print_bitmap(std::ostream& os, const BufferManager& buffer_manager) {
      unsigned int index = 0;
      while(index < buffer_manager.mem_pool_bitmap.size()) {
          if(buffer_manager.mem_pool_bitmap[index]) { os << "1"; } else { os << "0";}
          index++;
          if(index == buffer_manager.mem_pool_bitmap.size()) { break; }
          if(index && index % 32 == 0) { os << std::endl; } else { os << ":";}
      }

      return os;
  }

  void free_avail_pages() {
      try {
          while(true) {
            auto victim = replacement_alg->get_victim(mem_pool_bitmap, mem_pool);
            std::cout << "freeing avail page: " << victim << std::endl;
          }
      } catch(BufferManagerError& e) {
          std::cout << e.what() << std::endl;
      }
  }

  private:
  BufferPageCursor get_next_free_page();
  BufferPageCursor get_page_cursor(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void set_page_cursor(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor bufer_page_cursor);

  const std::shared_ptr<DiskManager> disk_manager;
  const std::shared_ptr<ReplacementAlg> replacement_alg;

  // NOTE: tracks allocated pages
  std::vector<bool> mem_pool_bitmap;
  // NOTE: maps disk pages to mem pool pages
  MemPoolMapType mem_pool_map;
  std::unique_ptr<uint8_t[]> mem_pool;
};

#endif // BUFFER_MANAGER_H
