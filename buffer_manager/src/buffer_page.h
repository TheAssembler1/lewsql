#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <disk_manager.h>
#include <cstdint>
#include <memory>

struct BufferPage {
  DiskPageCursor disk_page_cursor{0};
  DiskId disk_id{0};
  unsigned int pin_count{0};
  bool dirty{false};
  uint8_t bytes[];
};

#endif // BUFFER_PAGE_H
