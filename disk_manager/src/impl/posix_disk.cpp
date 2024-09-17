#include "posix_disk.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

using namespace DiskManager;

Result<void, DiskManagerError> PosixDisk::close() noexcept {
    should_close = false;
    errno = 0;

    assert(m_fd >= 0);

    LOG(LogLevel::TRACE) << "closing file with name " << m_file_path << std::endl;
    if(::close(m_fd) == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CLOSE_DISK_ERROR);
    }

    return VoidValue::Ok;
}

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path, bool should_exist) noexcept {
    errno = 0;
    int st = access(file_path.c_str(), F_OK);

    if(!should_exist) {
        if(st == -1 && errno != ENOENT) {
            LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        } else if(st != -1) {
            return DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
        }
    } else {
        if(st == -1 && errno == ENOENT) {
            return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
        } else if(st == -1) {
            LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
            return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        }
    }

    errno = 0;
    LOG(LogLevel::TRACE) << "opening file at path " << file_path << std::endl;
    int fd = open(file_path.c_str(), O_CREAT, 0666);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    LOG(LogLevel::TRACE) << "creating posix disk" << std::endl;
    return PosixDisk(file_path, fd);
}

PosixDisk::PosixDisk(PosixDisk&& posix_disk) noexcept {
    m_fd = posix_disk.m_fd;
    m_file_path = posix_disk.m_file_path;
    posix_disk.should_close = false;
}

PosixDisk& PosixDisk::operator=(PosixDisk&& posix_disk) noexcept {
    if(this != &posix_disk) {
        m_fd = posix_disk.m_fd;
        m_file_path = posix_disk.m_file_path;
        posix_disk.should_close = false;
    }

    return *this;
}

// NOTE: no way to indidate failures her so minimal work
//       should be done
PosixDisk::~PosixDisk() noexcept {
    if(should_close) {
        close();
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

Result<void, DiskManagerError> PosixDisk::extend() noexcept {
    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::destroy() noexcept {
    errno = 0;
    int stat = unlink(m_file_path.c_str());

    if(stat == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    should_close = false;
    return VoidValue::Ok;
}
