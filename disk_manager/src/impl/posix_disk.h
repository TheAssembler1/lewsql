#ifndef POSIX_DISK_H
#define POSIX_DISK_H

#include <logger.h>
#include <result.h>

#include "disk_manager_error.h"

namespace DiskManager {

class PosixDisk {
    Result<PosixDisk, DiskManagerError> init(const std::string& file_path) noexcept;

    PosixDisk(PosixDisk&& posix_disk) noexcept;
    PosixDisk& operator=(PosixDisk&& posix_disk) noexcept;

    ~PosixDisk() noexcept;

public:
    PosixDisk(const PosixDisk&) = delete;
    PosixDisk& operator=(const PosixDisk&) = delete;

    std::string get_file_path() const { return m_file_path; }
    int get_fd() const { return m_fd; }
private:
    PosixDisk(const std::string& file_path): m_file_path{file_path} {};

    std::string m_file_path;
    int m_fd;
};

}

#endif // POSIX_DISK_H
