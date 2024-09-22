#ifndef POSIX_DISK_H
#define POSIX_DISK_H

#include <logger.h>
#include <result.h>

#include "disk_manager_error.h"
#include "disk_manager_types.h"

namespace DiskManager {

class PosixDisk final {
    public:
    PosixDisk(const PosixDisk&) = delete;
    PosixDisk& operator=(const PosixDisk&) = delete;
    PosixDisk(PosixDisk&& posix_disk) noexcept;
    PosixDisk& operator=(PosixDisk&& posix_disk) noexcept;
    ~PosixDisk() noexcept;

    static Result<PosixDisk, DiskManagerError> init(const std::string& file_path, bool should_exist, unsigned int page_size) noexcept;

    Result<unsigned int, DiskManagerError> get_disk_size() const noexcept;
    Result<void, DiskManagerError> write(DiskPageCursor disk_page_cursor, const uint8_t* const data) noexcept;
    Result<void, DiskManagerError> read(DiskPageCursor disk_page_cursor, uint8_t* const data) noexcept;
    Result<void, DiskManagerError> destroy() noexcept;
    Result<void, DiskManagerError> close() noexcept;
    Result<void, DiskManagerError> sync() noexcept;

    std::string get_file_path() const {
        return m_file_path;
    }
    int get_fd() const {
        return m_fd;
    }

    private:
    PosixDisk(const std::string& file_path, int fd, unsigned int page_size):
      m_file_path{file_path}, m_fd{fd}, m_page_size{page_size} {
    }

    Result<void, DiskManagerError> prepare_rw(DiskPageCursor disk_page_cursor) noexcept;
    Result<void, DiskManagerError> truncate(unsigned int new_byte_size) noexcept;
    Result<void, DiskManagerError> seek(unsigned int pos) noexcept;

    std::string m_file_path;
    int m_fd;
    unsigned int m_page_size;
    bool should_close = true;
};

} // namespace DiskManager

#endif // POSIX_DISK_H
