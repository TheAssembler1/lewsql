#include "posix_disk_manager.h"

#include <algorithm>

#include "disk_manager.h"
#include "disk_manager_error.h"

unsigned int PosixDiskManager::num_loaded_disk() const {
    return disks.size();
}

PosixDiskManager::PosixDiskManager(const std::string& dir_path)
: DiskManager{dir_path} {
    std::cout << "dir path set to path: " << dir_path << std::endl;
}

DiskId PosixDiskManager::d_create(const DiskName& disk_name) {
    std::string full_path_fname = get_disk_path(disk_name);

    // NOTE: check if file already exists
    errno = 0;
    int st = access(full_path_fname.c_str(), F_OK);

    // NOTE: access fails ensure it was due to it not existing
    if(st == -1) {
        if(errno != ENOENT) {
            std::cerr << "failed to access file, errno: " << std::strerror(errno) << std::endl;
            throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
        }
    } else { // NOTE: file already exists
        std::cerr << "file already exists" << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::DISK_ALREADY_EXISTS);
    }

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_CREAT, 0644);

    // FIXME: return more specific errors based on errno status
    if(fd == -1) {
        std::cerr << "failed to open file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    } else {
        std::cout << "created file at path: " << full_path_fname << std::endl;
    }

    errno = 0;
    st = close(fd);

    if(st == -1) {
        std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::CREATE_DISK_ERROR);
    }

    return d_load(disk_name);
}

void PosixDiskManager::d_destroy(DiskId disk_id) {
    auto full_path_fname = get_disk_path(disk_loaded(disk_id));

    errno = 0;
    int stat = unlink(full_path_fname.c_str());

    if(stat == -1) {
        std::cerr << "failed to unlink file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    d_unload(disk_id);
}

DiskId PosixDiskManager::d_load(const DiskName& disk_name) {
    std::string full_path_fname = get_disk_path(disk_name);

    // NOTE: check if file already exists
    errno = 0;
    int st = access(full_path_fname.c_str(), F_OK);

    // NOTE: access fails ensure it was due to it not existing
    if(st == -1) {
        std::cerr << "disk load could not find file: " << full_path_fname << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::UNKOWN_ERROR);
    }

    DiskId ret = cur_disk_id;
    disks[ret] = disk_name;
    cur_disk_id++;

    return ret;
}

void PosixDiskManager::d_unload(DiskId disk_id) {
    get_disk_path(disk_loaded(disk_id));
    disks.erase(disk_id);
}

void PosixDiskManager::d_write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes, unsigned int page_size) {
    std::string full_path_fname = get_disk_path(disk_loaded(disk_id));

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_WRONLY);

    if(fd == -1) {
        std::cerr << "failed to open file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    // FIXME: validation on this number
    int foffset = disk_page_cursor * page_size;

    errno = 0;
    int res = lseek(fd, foffset, SEEK_SET);

    if(res == -1) {
        std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    std::cout << "writing (foffset, num_bytes) = "
              << "(" << foffset << ", " << page_size << ")" << std::endl;    unsigned int num_bytes = write(fd, bytes, page_size);

    if(num_bytes != page_size) {
        std::cerr << "failed to write num_bytes to file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    res = fsync(fd);
    if(res == -1) {
        std::cerr << "failed to fsync file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);
    if(res == -1) {
        std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }
}

void PosixDiskManager::d_read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes, unsigned int page_size) {
    auto full_path_fname = get_disk_path(disk_loaded(disk_id));

    // FIXME: ensuring file is large enough
    struct stat st;
    errno = 0;
    int res = stat(full_path_fname.c_str(), &st);

    if(res == -1) {
        std::cerr << "failed to stat file at errno: " << strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_RDWR);

    if(fd == -1) {
        std::cerr << "failed to open file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    // NOTE: set file pointer
    unsigned int foffset = (disk_page_cursor + 1) * page_size - 1;
    std::cout << "file size found to be: " << st.st_size << std::endl;
    if(foffset > st.st_size) {
        std::cout << "foffset greater than file size... extending file" << std::endl;

        errno = 0;
        res = lseek(fd, foffset, SEEK_SET);
        if(res == -1) {
            std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
            throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }

        errno = 0;
        int byte = 0;

        std::cout << "extending file to offset: " << foffset << std::endl;
        res = write(fd, &byte, 1);

        if(res == -1) {
            std::cerr << "failed to write file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
            throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }

        errno = 0;
        res = fsync(fd);
        if(res == -1) {
            std::cerr << "failed to fsync file, errno: " << std::strerror(errno) << std::endl;
            throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
        }
    }

    // NOTE: reset file pointer
    foffset = disk_page_cursor * page_size;
    errno = 0;
    res = lseek(fd, foffset, SEEK_SET);
    if(res == -1) {
        std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    std::cout << "reading (foffset, num_bytes) = "
              << "(" << foffset << ", " << page_size << ")" << std::endl;
    unsigned int num_bytes = read(fd, bytes, page_size);

    if(num_bytes != page_size) {
        std::cerr << "failed to read num_bytes to file, errno: " << std::strerror(errno) << " num bytes: " << num_bytes
                  << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);

    if(res == -1) {
        std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }
}

std::ostream& operator<<(std::ostream& os, const PosixDiskManager& posix_disk_manager) {
    os << "total loaded disks: " << posix_disk_manager.disks.size();

    if(posix_disk_manager.disks.size() > 0) {
        os << std::endl;
    }

    int cur = 0;
    for(const auto [disk_id, disk_name]: posix_disk_manager.disks) {
        os << "(disk_id, disk_name) = (" << disk_id << ", " << disk_name << ")";
        cur++;

        if(cur != posix_disk_manager.disks.size()) {
            os << std::endl;
        }
    }

    return os;
}

DiskName PosixDiskManager::disk_loaded(DiskId disk_id) {
    auto it = disks.find(disk_id);

    if(it == disks.end()) {
        throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
    }

    return it->second;
};

unsigned int PosixDiskManager::disk_size(DiskId disk_id) {
    auto full_path_fname = get_disk_path(disk_loaded(disk_id));

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_RDWR);

    // FIXME: ensuring file is large enough
    struct stat st;
    errno = 0;
    int res = stat(full_path_fname.c_str(), &st);

    if(res == -1) {
        std::cerr << "failed to stat file at errno: " << strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    res = close(fd);

    if(res == -1) {
        std::cerr << "failed to close file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    return st.st_size;
}
