#include "posix_disk.h"

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "disk_manager_types.h"

using namespace DiskManager;

Result<void, DiskManagerError> PosixDisk::close() noexcept {
    should_close = false;
    errno = 0;

    assert(m_fd >= 0);

    if(::close(m_fd) == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CLOSE_DISK_ERROR);
    }

    return VoidValue::Ok;
}

Result<bool, DiskManagerError> PosixDisk::exists(const std::string& file_path) noexcept {
    errno = 0;
    int st = access(file_path.c_str(), F_OK);

    if(st == 0) {
        return true;
    }

    if(errno == ENOENT) {
        return false;
    }

    LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
    return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
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

Result<PosixDisk, DiskManagerError> PosixDisk::init(const std::string& file_path, bool should_exist, unsigned int page_size, unsigned int max_disk_size) noexcept {
    auto exist = UNWRAP_OR_PROP_ERROR(PosixDisk::exists(file_path));

    if(!should_exist && exist) {
        return DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
    }

    if(should_exist && !exist) {
        return DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
    }

    errno = 0;
    int fd = open(file_path.c_str(), O_RDWR | O_CREAT , 0666);

    if(fd == -1) {
        LOG(LogLevel::ERROR) << std::strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    return PosixDisk(file_path, fd, page_size, max_disk_size);
}

PosixDisk::PosixDisk(PosixDisk&& posix_disk) noexcept {
    m_fd = posix_disk.m_fd;
    m_file_path = posix_disk.m_file_path;
    m_page_size = posix_disk.m_page_size;
    m_max_disk_size = posix_disk.m_max_disk_size;

    posix_disk.should_close = false;
}

PosixDisk& PosixDisk::operator=(PosixDisk&& posix_disk) noexcept {
    if(this != &posix_disk) {
        m_fd = posix_disk.m_fd;
        m_file_path = posix_disk.m_file_path;
        m_page_size = posix_disk.m_page_size;
        m_max_disk_size = posix_disk.m_max_disk_size;

        posix_disk.should_close = false;
    }

    return *this;
}

PosixDisk::~PosixDisk() noexcept {
    if(should_close) { close(); }
}

Result<unsigned int, DiskManagerError> PosixDisk::get_disk_size() const noexcept {
    errno = 0;
    struct stat st;
    int res = stat(m_file_path.c_str(), &st);

    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::GET_DISK_SIZE_ERROR);
    }

    if(st.st_size % m_page_size != 0) {
        LOG(LogLevel::INFO) << "file size: " << st.st_size << std::endl;
        LOG(LogLevel::FATAL) << "file size not multiple of page size" << std::endl;
        assert(0);
    }

    return st.st_size;
}

Result<void, DiskManagerError> PosixDisk::prepare_rw(DiskPageCursor disk_page_cursor) noexcept {
    auto cur_byte_size = UNWRAP_OR_PROP_ERROR(get_disk_size());
    auto foffset = disk_page_cursor * m_page_size;

    if(foffset + m_page_size >= cur_byte_size) {
        LOG(LogLevel::WARNING) << "rw operation exceeds disk size" << std::endl;
        truncate(foffset + m_page_size);
    }

    PROP_IF_ERROR(seek(foffset));

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::write(DiskPageCursor disk_page_cursor, const uint8_t* const data) noexcept {
    PROP_IF_ERROR(prepare_rw(disk_page_cursor));

    errno = 0;
    unsigned int num_bytes = ::write(m_fd, data, m_page_size);

    LOG(LogLevel::INFO) << "resulting file size would be: " << num_bytes << std::endl;

    if(num_bytes != m_page_size) {
        if(num_bytes >= m_max_disk_size) {
            return DiskManagerError(DiskManagerErrorCode::DISK_EXCEEDS_MAX_SIZE);
        }

        return DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    PROP_IF_ERROR(sync());

    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::read(DiskPageCursor disk_page_cursor, uint8_t* const data) noexcept {
    PROP_IF_ERROR(prepare_rw(disk_page_cursor));

    errno = 0;
    unsigned int num_bytes = ::read(m_fd, data, m_page_size);
    if(num_bytes != m_page_size) {
        return DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }


    return VoidValue::Ok;
}

Result<void, DiskManagerError> PosixDisk::truncate(unsigned new_byte_size) noexcept {
    errno = 0;
    int res = ::truncate(m_file_path.c_str(), new_byte_size);
    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    PROP_IF_ERROR(sync());

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

Result<void, DiskManagerError> PosixDisk::sync() noexcept {
    errno = 0;
    int res = fsync(m_fd);
    if(res == -1) {
        LOG(LogLevel::ERROR) << strerror(errno) << std::endl;
        return DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    return VoidValue::Ok;
}
