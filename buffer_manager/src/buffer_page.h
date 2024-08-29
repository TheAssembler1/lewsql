#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <cstdint>
#include <memory>

class BufferPage final {
  public:
  BufferPage(unsigned int num_bytes): bytes{new uint8_t[num_bytes]}  {}

  std::unique_ptr<uint8_t[]> bytes;
  unsigned int pin_count{0};
  bool dirty{false};
};

#endif // BUFFER_PAGE_H
