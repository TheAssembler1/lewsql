#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <unordered_map>
#include <vector>

class BufferManager final {

  std::unordered_map<unsigned int, std::vector<std::byte>> buffer_map;
};

#endif // BUFFER_MANAGER_H
