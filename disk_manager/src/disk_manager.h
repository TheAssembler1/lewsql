#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <cassert>
#include <result.h>
#include <string>

#include "disk_manager_error.h"
#include "disk_manager_types.h"

namespace DiskManager {

class DiskManager {
    public:
    DiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_byte_size)
    : dir_path{dir_path}, page_size{page_size}, max_disk_byte_size{max_disk_byte_size} {
    }

    virtual Result<DiskId, DiskManagerError> create(const DiskName& disk_name_prefix) noexcept = 0;
    virtual Result<void, DiskManagerError> destroy(DiskId disk_id) noexcept = 0;

    virtual Result<DiskId, DiskManagerError> load(const DiskName& disk_name_prefix) noexcept = 0;
    virtual Result<void, DiskManagerError> unload(DiskId disk_id) noexcept = 0;

    virtual Result<unsigned int, DiskManagerError> num_loaded_disks() const noexcept = 0;

    virtual Result<void, DiskManagerError> write(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) noexcept = 0;
    virtual Result<void, DiskManagerError> read(DiskId disk_id, DiskPageCursor page_cursor, uint8_t* bytes) noexcept = 0;
    virtual Result<DiskPageCursor, DiskManagerError> extend(DiskId disk_id) noexcept = 0;

    virtual Result<DiskName, DiskManagerError> loaded_disk_name(DiskId disk_id) noexcept = 0;
    virtual Result<unsigned int, DiskManagerError> disk_byte_size(DiskId disk_id) noexcept = 0;

    virtual Result<bool, DiskManagerError> exists(const std::string& disk_name_prefix) noexcept = 0;
    virtual Result<bool, DiskManagerError> is_loaded(DiskId disk_id) noexcept = 0;

    virtual std::string get_disk_full_name(const std::string& disk_name_prefix) const noexcept = 0;

    const std::string& get_dir_path() {
        return dir_path;
    }

    const std::string get_disk_suffix() const {
        return disk_suffix;
    }

    unsigned int get_page_size() const {
        return page_size;
    }

    unsigned int get_max_disk_byte_size() const {
        return max_disk_byte_size;
    }

    private:
    const std::string dir_path;
    const std::string disk_suffix{".disk"};
    const unsigned int page_size;
    const unsigned int max_disk_byte_size;
};

} // namespace DiskManager

#endif // DISK_MANAGER_H
