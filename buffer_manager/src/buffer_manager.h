#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <algorithm>
#include <cassert>
#include <cstring>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "buffer_manager_types.h"
#include "buffer_page.h"
#include "buffer_page_tracker/buffer_page_tracker.h"
#include "memory_pool.h"
#include "replacement/replacement_alg.h"

class BufferManager final {
    public:
    BufferManager(std::shared_ptr<DiskManager::DiskManager> disk_manager,
    std::unique_ptr<ReplacementAlg> replacement_alg,
    std::unique_ptr<BufferPageTracker> buffer_page_tracker,
    unsigned int num_pages,
    unsigned int page_size)
    : page_size{page_size}, disk_manager{disk_manager}, replacement_alg{std::move(replacement_alg)},
      buffer_page_tracker{std::move(buffer_page_tracker)}, mem_pool{num_pages, page_size} {
    }

    BufferPage& pin(DiskId disk_id, DiskPageCursor disk_page_cursor);
    void unpin(DiskId disk_id, DiskPageCursor disk_page_cursor);
    void set_dirty(DiskId disk_id, DiskPageCursor disk_page_cursor);

    void add_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor, BufferPageCursor buffer_page_cursor);
    void remove_page_mem_pool_map(BufferPageCursor victim_page_cursor);
    BufferPageCursor get_page_mem_pool_map(DiskId disk_id, DiskPageCursor disk_page_cursor) const;

    std::ostream& print_mem_pool_map(std::ostream& os, const BufferManager& buffer_manager);

    void free_avail_pages();

    // NOTE: maps disk pages to mem pool pages
    MemPoolMapType mem_pool_map;
    MemoryPool mem_pool;

    const std::shared_ptr<DiskManager::DiskManager> disk_manager;
    const std::unique_ptr<ReplacementAlg> replacement_alg;
    const std::unique_ptr<BufferPageTracker> buffer_page_tracker;

    private:
    const unsigned int page_size;
};

#endif // BUFFER_MANAGER_H
