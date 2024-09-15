#include "posix_disk.h"

#include <sys/unistd.h>
#include <sys/errno.h>
#include <fcntl.h>

using namespace DiskManager;

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path) noexcept {
    PosixDisk posix_disk{file_path};

    errno = 0;
    int fd = open(file_path.c_str(), O_RDWR);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << "failed to open file, errno: "
                             << std::strerror(errno) << " file path: " << file_path << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    return std::move(posix_disk);
}

PosixDisk::PosixDisk(PosixDisk&& posix_disk) noexcept {
    if(this != &posix_disk) {
        m_fd = posix_disk.m_fd;
        m_file_path = posix_disk.m_file_path;

        posix_disk.m_fd = -1;
    }
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
            LOG(LogLevel::ERROR) << "failed to close file, errno: "
                                 << std::strerror(errno) << std::endl;
        }
    }
}

