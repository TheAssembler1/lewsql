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

#include "replacement/replacement_alg.h"
#include "buffer_manager_types.h"

class BufferManager final {
  public:
  BufferManager(std::shared_ptr<DiskManager> disk_manager,
                std::shared_ptr<ReplacementAlg> replacement_alg,
                unsigned int num_pages)
    : disk_manager{disk_manager},
      replacement_alg{replacement_alg},
      mem_pool_bitmap(num_pages) {
    assert(num_pages > 0 && disk_manager->page_size > 0);
  }

  uint8_t* pin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void unpin(DiskId disk_id, DiskPageCursor disk_page_cursor);
  void set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor);

  BufferPageCursor get_next_free_page();

  private:
  const std::shared_ptr<DiskManager> disk_manager;
  const std::shared_ptr<ReplacementAlg> replacement_alg;

  // NOTE: tracks allocated pages
  std::vector<bool> mem_pool_bitmap;

  // NOTE: maps disk pages to mem pool pages
  MemPoolMapType mem_pool_map;

  std::unique_ptr<uint8_t[]> mem_pool;
};

#endif // BUFFER_MANAGER_H
