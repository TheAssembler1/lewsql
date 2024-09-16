#include "posix_disk.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>

using namespace DiskManager;

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path) noexcept {
    PosixDisk posix_disk{file_path};

    errno = 0;
    int fd = open(file_path.c_str(), O_RDWR);

    if(fd == -1) {
        if(errno == ENOENT) {
            return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
        }

        LOG(LogLevel::ERROR) << "failed to open file, errno: " << std::strerror(errno) << " file path: " << file_path << std::endl;
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
            LOG(LogLevel::ERROR) << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        }
    }
}

Result<unsigned int, DiskManagerError> PosixDisk::get_disk_size() const noexcept {
    return 0;
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
        LOG(LogLevel::ERROR) << "failed to unlink file, errno: " << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    m_fd = -1;

    return VoidValue::Ok;
}
