#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <cassert>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <result.h>

#include "disk_manager_error.h"
#include "disk_manager_types.h"

namespace DiskManager {

class DiskManager {
    public:
    DiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size):
        dir_path{dir_path}, page_size{page_size}, max_disk_size{max_disk_size} {
    }

    virtual Result<DiskId, DiskManagerError> create(const DiskName& disk_name) noexcept = 0;
    virtual Result<void, DiskManagerError> destroy(DiskId disk_id) noexcept = 0;

    virtual Result<DiskId, DiskManagerError> load(const DiskName& disk_name) noexcept = 0;
    virtual Result<void, DiskManagerError> unload(DiskId disk_id) noexcept = 0;

    virtual Result<unsigned int, DiskManagerError> num_loaded_disks() const noexcept = 0;

    virtual Result<void, DiskManagerError> write(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) noexcept = 0;
    virtual Result<void, DiskManagerError> read(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) noexcept = 0;
    virtual Result<DiskPageCursor, DiskManagerError> extend(DiskId disk_id) noexcept = 0;

    virtual Result<DiskName, DiskManagerError> loaded_disk_name(DiskId disk_id) noexcept = 0;
    virtual Result<unsigned int, DiskManagerError> disk_size(DiskId disk_id) noexcept = 0;

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
