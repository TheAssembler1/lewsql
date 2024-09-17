#ifndef POSIX_DISK_H
#define POSIX_DISK_H

#include <logger.h>
#include <result.h>

#include "disk_manager_error.h"

namespace DiskManager {

class PosixDisk {
    public:
    PosixDisk(const PosixDisk&) = delete;
    PosixDisk& operator=(const PosixDisk&) = delete;
    PosixDisk(PosixDisk&& posix_disk) noexcept;
    PosixDisk& operator=(PosixDisk&& posix_disk) noexcept;
    ~PosixDisk() noexcept;

    static Result<PosixDisk, DiskManagerError> init(const std::string& file_path, bool should_exist) noexcept;

    Result<unsigned int, DiskManagerError> get_disk_size() const noexcept;
    Result<void, DiskManagerError> write() noexcept;
    Result<void, DiskManagerError> read() noexcept;
    Result<void, DiskManagerError> extend() noexcept;
    Result<void, DiskManagerError> destroy() noexcept;
    Result<void, DiskManagerError> close() noexcept;

    std::string get_file_path() const {
        return m_file_path;
    }
    int get_fd() const {
        return m_fd;
    }

    private:
    PosixDisk(const std::string& file_path, int fd) : m_file_path{file_path}, m_fd{fd} {
        LOG(LogLevel::TRACE) << "creating posix disk with file path: " << m_file_path << std::endl;
        LOG(LogLevel::TRACE) << "creating posix disk with fd: " << m_fd << std::endl;
    }

    std::string m_file_path;
    int m_fd;
    bool should_close = true;
};

} // namespace DiskManager

#endif // POSIX_DISK_H
