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

    virtual DiskId create(const DiskName& disk_name) override;
    virtual void destroy(DiskId disk_id) override;

    virtual DiskId load(const DiskName& disk_name) override;
    virtual void unload(DiskId disk_id) override;
    virtual DiskPageCursor extend(DiskId disk_id) override;

    virtual unsigned int num_loaded_disks() const override;

    virtual void write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) override;
    virtual void read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) override;

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
