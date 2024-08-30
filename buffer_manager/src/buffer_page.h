#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <cstdint>
#include <memory>

struct BufferPage {
  public:
  unsigned int pin_count{0};
  bool dirty{false};
  uint8_t bytes[];
};

#endif // BUFFER_PAGE_H
