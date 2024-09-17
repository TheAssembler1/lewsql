#include "posix_disk.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

using namespace DiskManager;

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path, bool should_exist) noexcept {
    errno = 0;
    int st = access(file_path.c_str(), F_OK);

    if(!should_exist) {
        if(st != -1) {
            return DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
        } else if(st == -1 && errno != ENOENT) {
            LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        }
    } else {
        if(st == -1 && errno == ENOENT) {
            return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
        } else if(st == -1) {
            LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        }
    }

    PosixDisk posix_disk{file_path};

    errno = 0;
    int fd = open(file_path.c_str(), O_RDWR);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    return posix_disk;
}

PosixDisk::PosixDisk(PosixDisk&& posix_disk) noexcept {
    m_fd = posix_disk.m_fd;
    m_file_path = posix_disk.m_file_path;

    posix_disk.m_fd = -1;
}

PosixDisk& PosixDisk::operator=(PosixDisk&& posix_disk) noexcept {
    if(this != &posix_disk) {
        m_fd = posix_disk.m_fd;
        m_file_path = posix_disk.m_file_path;
    }

    return *this;
}

// NOTE: no way to indidate failures here so minimal work
//       should be done
PosixDisk::~PosixDisk() noexcept {
    if(m_fd >= 0) {
        errno = 0;

        if(close(m_fd) == -1) {
            LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        }
    }
}

Result<unsigned int, DiskManagerError> PosixDisk::get_disk_size() const noexcept {
    errno = 0;
    struct stat st;
    int res = stat(m_file_path.c_str(), &st);

    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::GET_DISK_SIZE_ERROR);
    }

    return st.st_size;
}

Result<void, DiskManagerError> PosixDisk::write() noexcept {
    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::read() noexcept {
    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::destroy() noexcept {
    errno = 0;
    int stat = unlink(m_file_path.c_str());

    if(stat == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    m_fd = -1;

    return VoidValue::Ok;
}
