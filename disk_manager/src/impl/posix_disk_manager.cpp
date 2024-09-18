#include "posix_disk_manager.h"

#include <logger.h>

#include "disk_manager.h"
#include "disk_manager_error.h"
#include "posix_disk.h"

namespace DiskManager {

PosixDiskManager::PosixDiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size)
: DiskManager{dir_path, page_size, max_disk_size} {
    LOG(LogLevel::INFO) << "dir path set to path: " << dir_path << std::endl;
}

Result<unsigned int, DiskManagerError> PosixDiskManager::num_loaded_disks() const noexcept {
    return disks.size();
}

Result<DiskId, DiskManagerError> PosixDiskManager::create(const DiskName& disk_name) noexcept {
    std::string path = get_disk_path(disk_name);

    PROP_IF_ERROR(PosixDisk::init(path, false, get_page_size()));

    return load(disk_name);
}

Result<void, DiskManagerError> PosixDiskManager::destroy(DiskId disk_id) noexcept {
    std::string file_path = UNWRAP_OR_PROP_ERROR(get_disk_path(disk_id));
    auto posix_disk_res = PosixDisk::init(file_path, true, get_page_size());

    PROP_IF_ERROR(posix_disk_res);
    PROP_IF_ERROR(posix_disk_res.get_value().destroy());

    return VoidValue::Ok;
}

Result<DiskId, DiskManagerError> PosixDiskManager::load(const DiskName& disk_name) noexcept {
    std::string path = get_disk_path(disk_name);
    auto posix_disk_res = PosixDisk::init(path, true, get_page_size());
    PROP_IF_ERROR(posix_disk_res);

    DiskId ret = cur_disk_id;
    disks[ret] = disk_name;
    cur_disk_id++;

    return ret;
}

Result<void, DiskManagerError> PosixDiskManager::unload(DiskId disk_id) noexcept {
    if(!disks.erase(disk_id)) {
        return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
    }

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDiskManager::write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) noexcept {
    std::string path = UNWRAP_OR_PROP_ERROR(get_disk_path(disk_id));
    auto posix_disk_res = PosixDisk::init(path, true, get_page_size());
    PROP_IF_ERROR(posix_disk_res);

    auto& posix_disk = posix_disk_res.get_value();
    PROP_IF_ERROR(posix_disk.write(disk_page_cursor, bytes));

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDiskManager::read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) noexcept {
    std::string path = UNWRAP_OR_PROP_ERROR(get_disk_path(disk_id));
    auto posix_disk_res = PosixDisk::init(path, true, get_page_size());
    PROP_IF_ERROR(posix_disk_res);

    auto& posix_disk = posix_disk_res.get_value();
    PROP_IF_ERROR(posix_disk.read(disk_page_cursor, bytes));

    return VoidValue::Ok;
}

std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager) {
    os << "total loaded disks: " << posix_disk_manager.disks.size();

    if(posix_disk_manager.disks.size() > 0) {
        os << std::endl;
    }

    int cur = 0;
    for(const auto& [disk_id, disk_name] : posix_disk_manager.disks) {
        os << "(disk_id, disk_name) = (" << disk_id << ", " << disk_name << ")";
        cur++;

        if(cur != posix_disk_manager.disks.size()) {
            os << std::endl;
        }
    }

    return os;
}

Result<DiskName, DiskManagerError> PosixDiskManager::loaded_disk_name(DiskId disk_id) noexcept {
    auto it = disks.find(disk_id);

    if(it == disks.end()) {
        return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
    }

    return it->second;
};

Result<DiskPageCursor, DiskManagerError> PosixDiskManager::extend(DiskId disk_id) noexcept {
    unsigned int size = UNWRAP_OR_PROP_ERROR(disk_size(disk_id));
    assert(size % get_page_size() == 0);

    if(size > get_max_disk_size()) {
        return DiskManagerError(DiskManagerErrorCode::DISK_EXCEEDS_MAX_SIZE);
    }

    return static_cast<DiskPageCursor>((size / get_page_size()) + 1);
}

std::string PosixDiskManager::get_disk_path(const std::string& file_name) {
    return get_dir_path() + "/" + file_name;
}

Result<std::string, DiskManagerError> PosixDiskManager::get_disk_path(DiskId disk_id) {
    auto res = UNWRAP_OR_PROP_ERROR(loaded_disk_name(disk_id));
    return get_disk_path(res);
}

Result<unsigned int, DiskManagerError> PosixDiskManager::disk_size(DiskId disk_id) noexcept {
    auto path = UNWRAP_OR_PROP_ERROR(get_disk_path(disk_id));
    auto posix_disk_res = PosixDisk::init(path, true, get_page_size());

    PROP_IF_ERROR(posix_disk_res);

    const auto& posix_disk = posix_disk_res.get_value();
    return posix_disk.get_disk_size();
}

} // namespace DiskManager
