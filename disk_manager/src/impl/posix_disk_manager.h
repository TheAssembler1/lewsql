#ifndef POSIX_DISK_MANAGER_H
#define POSIX_DISK_MANAGER_H

#include <iterator>
#ifdef __unix__
#include <cerrno>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#else
#error "POSIX compliance required"
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <disk_manager.h>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

namespace DiskManager {

class PosixDiskManager final: public DiskManager {
    public:
    PosixDiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size);

    friend std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager);

    virtual Result<DiskId, DiskManagerError> create(const DiskName& disk_name) override;
    virtual Result<void, DiskManagerError> destroy(DiskId disk_id) override;

    virtual Result<DiskId, DiskManagerError> load(const DiskName& disk_name) override;
    virtual Result<void, DiskManagerError> unload(DiskId disk_id) override;
    virtual Result<DiskPageCursor, DiskManagerError> extend(DiskId disk_id) override;

    virtual Result<unsigned int, DiskManagerError> num_loaded_disks() const override;

    virtual Result<void, DiskManagerError> write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) override;
    virtual Result<void, DiskManagerError> read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) override;

    virtual DiskName loaded_disk_name(DiskId disk_id) override;
    virtual unsigned int disk_size(DiskId disk_id) override;

    private:
    std::string get_disk_path(const DiskName& disk_name) {
        return get_dir_path() + "/" + disk_name;
    }
    DiskId cur_disk_id = 0;

    std::unordered_map<DiskId, DiskName> disks{};
};

}

#endif // POSIX_DISK_MANAGER_H
