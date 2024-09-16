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

    static Result<PosixDisk, DiskManagerError> init(const std::string& file_path) noexcept;

    Result<unsigned int, DiskManagerError> get_disk_size() const noexcept;
    Result<void, DiskManagerError> write() noexcept;
    Result<void, DiskManagerError> read() noexcept;
    Result<void, DiskManagerError> destroy() noexcept;

    std::string get_file_path() const {
        return m_file_path;
    }
    int get_fd() const {
        return m_fd;
    }

    private:
    PosixDisk(const std::string& file_path) : m_file_path{file_path} {};

    std::string m_file_path;
    int m_fd;
};

} // namespace DiskManager

#endif // POSIX_DISK_H
