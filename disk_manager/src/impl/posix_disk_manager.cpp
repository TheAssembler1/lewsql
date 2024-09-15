#include "posix_disk_manager.h"

#include <algorithm>
#include <logger.h>

#include "disk_manager.h"
#include "disk_manager_error.h"

namespace DiskManager {

PosixDiskManager::PosixDiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size):
  DiskManager{dir_path, page_size, max_disk_size} {
    LOG(LogLevel::INFO) << "dir path set to path: " << dir_path << std::endl;
}

Result<unsigned int, DiskManagerError> PosixDiskManager::num_loaded_disks() const noexcept {
    return disks.size();
}

Result<DiskId, DiskManagerError> PosixDiskManager::create(const DiskName& disk_name) noexcept {
    std::string full_path_fname = get_disk_path(disk_name);

    // NOTE: check if file already exists
    errno = 0;
    int st = access(full_path_fname.c_str(), F_OK);

    // NOTE: access fails ensure it was due to it not existing
    if(st == -1) {
        if(errno != ENOENT) {
            LOG(LogLevel::ERROR) << "failed to access file, errno: " << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        }
    } else { // NOTE: file already exists
        return DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
    }

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_CREAT, 0644);

    // FIXME: return more specific errors based on errno status
    if(fd == -1) {
        LOG(LogLevel::ERROR) << "failed to open file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    } else {
        LOG(LogLevel::INFO) << "created file at path: " << full_path_fname << std::endl;
    }

    errno = 0;
    st = close(fd);

    if(st == -1) {
        LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    return load(disk_name);
}

Result<void, DiskManagerError> PosixDiskManager::destroy(DiskId disk_id) noexcept {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id).get_value());

    errno = 0;
    int stat = unlink(full_path_fname.c_str());

    if(stat == -1) {
        LOG(LogLevel::ERROR) << "failed to unlink file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    unload(disk_id);

    return VoidValue::Ok;
}

Result<DiskId, DiskManagerError> PosixDiskManager::load(const DiskName& disk_name) noexcept {
    std::string full_path_fname = get_disk_path(disk_name);

    // NOTE: check if file already exists
    errno = 0;
    int st = access(full_path_fname.c_str(), F_OK);

    // NOTE: access fails ensure it was due to it not existing
    if(st == -1) {
        LOG(LogLevel::ERROR) << "disk load could not find file: " << full_path_fname << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    DiskId ret = cur_disk_id;
    disks[ret] = disk_name;
    cur_disk_id++;

    return ret;
}

Result<void, DiskManagerError> PosixDiskManager::unload(DiskId disk_id) noexcept {
    get_disk_path(loaded_disk_name(disk_id).get_value());
    disks.erase(disk_id);

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDiskManager::write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) noexcept {
    std::string full_path_fname = get_disk_path(loaded_disk_name(disk_id).get_value());

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_WRONLY);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << "failed to open file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    // FIXME: validation on this number
    int foffset = disk_page_cursor * get_page_size();

    errno = 0;
    int res = lseek(fd, foffset, SEEK_SET);

    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    LOG(LogLevel::INFO) << "writing (foffset, num_bytes) = "
              << "(" << foffset << ", " << get_page_size() << ")" << std::endl;
    unsigned int num_bytes = ::write(fd, bytes, get_page_size());

    if(num_bytes != get_page_size()) {
        LOG(LogLevel::ERROR) << "failed to write num_bytes to file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    res = fsync(fd);
    if(res == -1) {
        LOG(LogLevel::ERROR)<< "failed to fsync file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);
    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDiskManager::read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) noexcept {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id).get_value());

    struct stat st;
    errno = 0;
    int res = stat(full_path_fname.c_str(), &st);

    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to stat file at errno: " << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_RDWR);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << "failed to open file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    // NOTE: set file pointer
    assert(st.st_size % get_page_size() == 0);

    unsigned int foffset = (disk_page_cursor + 1) * get_page_size() - 1;
    LOG(LogLevel::INFO) << "file size found to be: " << st.st_size << std::endl;
    if(foffset > st.st_size) {
        LOG(LogLevel::INFO) << "foffset greater than file size... extending file" << std::endl;

        if(foffset >= get_max_disk_size()) {
            LOG(LogLevel::WARNING) << "foffset exceed max disk size" << std::endl;

            errno = 0;
            res = close(fd);
            if(res == -1) {
                LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
                return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
            }

            return DiskManagerError(DiskManagerErrorCode::DISK_EXCEEDS_MAX_SIZE);
        }

        errno = 0;
        res = lseek(fd, foffset, SEEK_SET);
        if(res == -1) {
            LOG(LogLevel::ERROR) << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }

        errno = 0;
        int byte = 0;

        LOG(LogLevel::INFO) << "extending file to offset: " << foffset << std::endl;
        res = ::write(fd, &byte, 1);

        if(res == -1) {
            LOG(LogLevel::ERROR) << "failed to write file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
            return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }

        errno = 0;
        res = fsync(fd);
        if(res == -1) {
            LOG(LogLevel::ERROR) << "failed to fsync file, errno: " << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }
    }

    // NOTE: reset file pointer
    foffset = disk_page_cursor * get_page_size();
    errno = 0;
    res = lseek(fd, foffset, SEEK_SET);
    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    LOG(LogLevel::INFO) << "reading (foffset, num_bytes) = "
              << "(" << foffset << ", " << get_page_size() << ")" << std::endl;
    unsigned int num_bytes = ::read(fd, bytes, get_page_size());

    if(num_bytes != get_page_size() ) {
        LOG(LogLevel::ERROR) << "failed to read num_bytes to file, errno: " << std::strerror(errno) << " num bytes: " << num_bytes
                  << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);
    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    return VoidValue::Ok;
}

std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager) {
    os << "total loaded disks: " << posix_disk_manager.disks.size();

    if(posix_disk_manager.disks.size() > 0) {
        os << std::endl;
    }

    int cur = 0;
    for(const auto [disk_id, disk_name] : posix_disk_manager.disks) {
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
    unsigned int size = disk_size(disk_id).get_value();
    assert(size % get_page_size() == 0);

    if(size > get_max_disk_size()) {
        return DiskManagerError(DiskManagerErrorCode::DISK_EXCEEDS_MAX_SIZE);
    }

    return static_cast<DiskPageCursor>((size / get_page_size()) + 1);

}

Result<unsigned int, DiskManagerError> PosixDiskManager::disk_size(DiskId disk_id) noexcept {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id).get_value());

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_RDWR);

    // FIXME: ensuring file is large enough
    struct stat st;
    errno = 0;
    int res = stat(full_path_fname.c_str(), &st);

    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to stat file at errno: " << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);

    if(res == -1) {
        LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    return st.st_size;
}

}
