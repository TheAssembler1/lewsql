#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <vector>
#include <memory>

static void pool_deleter(uint8_t* pool) {
  delete[] pool;
}

class MemPool {
  public:
  unsigned int num_bytes;
  std::unique_ptr<uint8_t[], decltype(&pool_deleter)> pool;
  
  MemPool(unsigned int num_bytes) : num_bytes{num_bytes}, pool{new uint8_t[num_bytes], pool_deleter} {}
};

#endif // MEM_POOL_H
