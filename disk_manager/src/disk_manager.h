#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <cassert>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "disk_manager_types.h"

namespace DiskManager {

class DiskManager {
    public:
    DiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size):
        dir_path{dir_path}, page_size{page_size}, max_disk_size{max_disk_size} {
    }

    virtual DiskId create(const DiskName& disk_name) = 0;
    virtual void destroy(DiskId disk_id) = 0;

    virtual DiskId load(const DiskName& disk_name) = 0;
    virtual void unload(DiskId disk_id) = 0;

    virtual unsigned int num_loaded_disks() const = 0;

    virtual void write(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) = 0;
    virtual void read(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) = 0;
    virtual DiskPageCursor extend(DiskId disk_id) = 0;

    virtual DiskName loaded_disk_name(DiskId disk_id) = 0;
    virtual unsigned int disk_size(DiskId disk_id) = 0;

    const std::string& get_dir_path() { return dir_path; }
    unsigned int get_page_size() { return page_size; }
    unsigned int get_max_disk_size() { return max_disk_size; }

private:
    const std::string dir_path;
    const unsigned int page_size;
    const unsigned int max_disk_size;
};

}

#endif // DISK_MANAGER_H
