#ifndef BUFFER_MANAGER_TYPES_H
#define BUFFER_MANAGER_TYPES_H

#include <stdint.h>
#include <unordered_map>
#include <disk_manager.h>

using BufferPageCursor = unsigned int;
using BufferPageByteCursor = uint8_t*;
using MemPoolMapType = std::unordered_map<BufferPageCursor, std::unordered_map<DiskId, DiskPageCursor>>;

#endif // BUFFER_MANAGER_TYPES_H
