#ifndef DISK_MANAGER_TYPES_H
#define DISK_MANAGER_TYPES_H

#include <cstdint>
#include <string>

using DiskName = std::string;

using DiskId = int32_t;
using DiskPageCursor = int32_t;

struct DiskDescriptor {
    DiskId id;
    DiskPageCursor cursor;
};

#endif // DISK_MANAGER_TYPES_H
