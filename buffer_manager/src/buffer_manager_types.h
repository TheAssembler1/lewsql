#ifndef BUFFER_MANAGER_TYPES_H
#define BUFFER_MANAGER_TYPES_H

#include <unordered_map>
#include <stdint.h>

using BufferPageCursor = unsigned int;
using BufferPageByteCursor = uint8_t*;
using MemPoolMapType = std::unordered_map<unsigned int, std::unordered_map<unsigned int, unsigned int>>;

#endif // BUFFER_MANAGER_TYPES_H
