#ifndef BUFFER_PAGE_H
#define BUFFER_PAGE_H

#include <vector>

class BufferPage final {
  public:
  BufferPage(std::vector<std::byte> bytes): bytes{bytes}  {}
    
  unsigned int pin_count{0};
  std::vector<std::byte> bytes{};
  bool dirty{false};

  // FIMXE: validate index domain
  std::byte const& operator[](unsigned int i) const { return bytes[i]; }
};

#endif // BUFFER_PAGE_H
