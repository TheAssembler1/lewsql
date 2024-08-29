#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <cstdint>
#include <memory>

static void bytes_deleter(uint8_t* bytes) {
  delete[] bytes;
}

class BufferPage final {
  public:
  BufferPage(unsigned int num_bytes): bytes{new uint8_t[num_bytes], bytes_deleter}  {}

  std::unique_ptr<uint8_t[], decltype(&bytes_deleter)> bytes;
  unsigned int pin_count{0};
  bool dirty{false};
};

#endif // BUFFER_PAGE_H
