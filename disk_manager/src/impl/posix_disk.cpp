#include "posix_disk.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "disk_manager_types.h"

using namespace DiskManager;

Result<void, DiskManagerError> PosixDisk::close() noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

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

Result<void, DiskManagerError> PosixDisk::seek(unsigned int pos) noexcept {
    errno = 0;
    int res = lseek(m_fd, pos, SEEK_SET);
    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    return VoidValue::Ok;
}

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path, bool should_exist, unsigned int page_size) noexcept {
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
    int fd = open(file_path.c_str(), O_RDWR | O_CREAT , 0666);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    LOG(LogLevel::TRACE) << "setting page size: " << page_size << std::endl;
    return PosixDisk(file_path, fd, page_size);
}

PosixDisk::PosixDisk(PosixDisk&& posix_disk) noexcept {
    m_fd = posix_disk.m_fd;
    m_file_path = posix_disk.m_file_path;
    m_page_size = posix_disk.m_page_size;

    posix_disk.should_close = false;
}

PosixDisk& PosixDisk::operator=(PosixDisk&& posix_disk) noexcept {
    if(this != &posix_disk) {
        m_fd = posix_disk.m_fd;
        m_file_path = posix_disk.m_file_path;
        m_page_size = posix_disk.m_page_size;

        posix_disk.should_close = false;
    }

    return *this;
}

PosixDisk::~PosixDisk() noexcept {
    if(should_close) { close(); }
}

Result<unsigned int, DiskManagerError> PosixDisk::get_disk_size() const noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    errno = 0;
    struct stat st;
    int res = stat(m_file_path.c_str(), &st);

    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::GET_DISK_SIZE_ERROR);
    }

    LOG(LogLevel::TRACE) << "page size: " << m_page_size << std::endl;
    LOG(LogLevel::TRACE) << "st size: " << st.st_size << std::endl;
    assert(st.st_size % m_page_size == 0);

    LOG(LogLevel::TRACE) << "file size: " << st.st_size << std::endl;

    return st.st_size;
}

Result<void, DiskManagerError> PosixDisk::prepare_rw(DiskPageCursor disk_page_cursor) noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    auto cur_byte_size = UNWRAP_OR_PROP_ERROR(get_disk_size());
    auto foffset = disk_page_cursor * m_page_size;

    if(foffset + m_page_size >= cur_byte_size) {
        LOG(LogLevel::WARNING) << "write exceeds disk size" << std::endl;
        extend(foffset + m_page_size);
    }

    PROP_IF_ERROR(seek(foffset));

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::write(DiskPageCursor disk_page_cursor, const uint8_t* const data) noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    PROP_IF_ERROR(prepare_rw(disk_page_cursor));

    errno = 0;
    unsigned int num_bytes = ::write(m_fd, data, m_page_size);
    if(num_bytes != m_page_size) {
        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    PROP_IF_ERROR(sync());

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::read(DiskPageCursor disk_page_cursor, uint8_t* const data) noexcept {
    LOG(LogLevel::TRACE) << "disk page cursor: " << disk_page_cursor << std::endl;
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    PROP_IF_ERROR(prepare_rw(disk_page_cursor));

    errno = 0;
    unsigned int num_bytes = ::read(m_fd, data, m_page_size);
    if(num_bytes != m_page_size) {
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }


    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::extend(unsigned new_byte_size) noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    PROP_IF_ERROR(seek(new_byte_size));

    errno = 0;
    int byte = 0;
    int res = ::write(m_fd, &byte, 1);
    if(res != 1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    PROP_IF_ERROR(sync());

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::destroy() noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    errno = 0;
    int stat = unlink(m_file_path.c_str());

    if(stat == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    should_close = false;
    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::sync() noexcept {
    LOG(LogLevel::TRACE) << "fd: " << m_fd << std::endl;

    errno = 0;
    int res = fsync(m_fd);
    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    return VoidValue::Ok;
}
