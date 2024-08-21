#ifndef DMANAGER_H
#define DMANAGER_H

#include <iostream>
#include <vector>
#include <sys/stat.h>

class DiskManager {
  public:
    unsigned int page_size = 512;
    const std::string dir_path{};

    using DiskId = unsigned int;
    using PageId = unsigned int;

    DiskManager(std::string& dir_path) : dir_path{dir_path} {}

    DiskId dopen();
    void dclose(DiskId disk_id);
    void dwrite(DiskId, PageId page_id, std::vector<std::byte> page_buffer);
    std::vector<std::byte> dread(DiskId disk_id, PageId page_id);
};

#endif // DMANAGER_H
