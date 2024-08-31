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
#include "buffer_page.h"

class BufferManager final {
  public:
  BufferManager(std::shared_ptr<DiskManager> disk_manager,
                std::shared_ptr<ReplacementAlg> replacement_alg,
                unsigned int num_pages)
    : disk_manager{disk_manager},
      replacement_alg{replacement_alg},
      num_pages{num_pages},
      mem_pool_bitmap(num_pages),
      // NOTE: allocates pages plus BufferPage memers into a mem pool
      mem_pool{new uint8_t[(num_pages * disk_manager->page_size) + (num_pages * sizeof(BufferPage))]}{
    assert(num_pages > 0 && disk_manager->page_size > 0);
    std::cout << "creating buffer manager with (num_pages, page_size) = ("
              << num_pages << ", " << disk_manager->page_size << ")" << std::endl;
  }

  uint8_t* pin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void unpin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor);

  BufferPage* get_page_mem_pool(BufferPageCursor buffer_page_cursor) const;

  void add_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor buffer_page_cursor);
  void remove_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor);
  BufferPageCursor get_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor);

  static std::ostream& print_bitmap(std::ostream& os, const BufferManager& buffer_manager);
  static std::ostream& print_mem_pool_map(std::ostream& os, const BufferManager& buffer_manager);

  void free_avail_pages();

  private:
  BufferPageCursor get_next_free_page();

  const std::shared_ptr<DiskManager> disk_manager;
  const std::shared_ptr<ReplacementAlg> replacement_alg;
  const unsigned int num_pages;

  // NOTE: tracks allocated pages
  std::vector<bool> mem_pool_bitmap;
  // NOTE: maps disk pages to mem pool pages
  MemPoolMapType mem_pool_map;
  // NOTE: mem pool with buffer pages
  std::unique_ptr<uint8_t[]> mem_pool;
};

#endif // BUFFER_MANAGER_H
