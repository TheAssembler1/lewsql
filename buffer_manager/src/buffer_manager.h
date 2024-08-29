#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
#include <vector>

struct 

class BufferManager final {
  BufferManager() {
      buffer_pool = std::vector<std::vector<std::byte>>{512};
      buffer_queue = 
  }

  // NOTE: acts as a ordering stamp
  unsigned int cur_page = 0;
  std::vector<std::vector<std::byte>> buffer_pool;
  std::priority_queue<unsigned int> buffer_queue;
  std::unordered_map<unsigned int>
};

#endif // BUFFER_MANAGER_H
