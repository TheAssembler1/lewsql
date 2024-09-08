#include "posix_disk_manager.h"

#include <algorithm>

#include "disk_manager.h"
#include "disk_manager_error.h"

namespace DiskManager {

unsigned int PosixDiskManager::num_loaded_disks() const {
    return disks.size();
}

PosixDiskManager::PosixDiskManager(const std::string& dir_path, unsigned int page_size, unsigned int max_disk_size):
    DiskManager{dir_path, page_size, max_disk_size} {
    std::cout << "dir path set to path: " << dir_path << std::endl;
}

DiskId PosixDiskManager::create(const DiskName& disk_name) {
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

    return load(disk_name);
}

void PosixDiskManager::destroy(DiskId disk_id) {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id));

    errno = 0;
    int stat = unlink(full_path_fname.c_str());

    if(stat == -1) {
        std::cerr << "failed to unlink file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::DESTROY_DISK_ERROR);
    }

    unload(disk_id);
}

DiskId PosixDiskManager::load(const DiskName& disk_name) {
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

void PosixDiskManager::unload(DiskId disk_id) {
    get_disk_path(loaded_disk_name(disk_id));
    disks.erase(disk_id);
}

void PosixDiskManager::write(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) {
    std::string full_path_fname = get_disk_path(loaded_disk_name(disk_id));

    errno = 0;
    int fd = open(full_path_fname.c_str(), O_WRONLY);

    if(fd == -1) {
        std::cerr << "failed to open file, errno: " << std::strerror(errno) << " file path: " << full_path_fname << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    // FIXME: validation on this number
    int foffset = disk_page_cursor * get_page_size();

    errno = 0;
    int res = lseek(fd, foffset, SEEK_SET);

    if(res == -1) {
        std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::WRITE_DISK_ERROR);
    }

    errno = 0;
    std::cout << "writing (foffset, num_bytes) = "
              << "(" << foffset << ", " << get_page_size() << ")" << std::endl;
    unsigned int num_bytes = ::write(fd, bytes, get_page_size());

    if(num_bytes != get_page_size()) {
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

void PosixDiskManager::read(DiskId disk_id, DiskPageCursor disk_page_cursor, uint8_t* bytes) {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id));

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
    assert(st.st_size % get_page_size() == 0);

    unsigned int foffset = (disk_page_cursor + 1) * get_page_size() - 1;
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
        res = ::write(fd, &byte, 1);

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
    foffset = disk_page_cursor * get_page_size();
    errno = 0;
    res = lseek(fd, foffset, SEEK_SET);
    if(res == -1) {
        std::cerr << "failed to lseek file, errno: " << std::strerror(errno) << std::endl;
        throw DiskManagerError(DiskManagerErrorCode::READ_DISK_ERROR);
    }

    errno = 0;
    std::cout << "reading (foffset, num_bytes) = "
              << "(" << foffset << ", " << get_page_size() << ")" << std::endl;
    unsigned int num_bytes = ::read(fd, bytes, get_page_size());

    if(num_bytes != get_page_size() ) {
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
    for(const auto [disk_id, disk_name] : posix_disk_manager.disks) {
        os << "(disk_id, disk_name) = (" << disk_id << ", " << disk_name << ")";
        cur++;

        if(cur != posix_disk_manager.disks.size()) {
            os << std::endl;
        }
    }

    return os;
}

DiskName PosixDiskManager::loaded_disk_name(DiskId disk_id) {
    auto it = disks.find(disk_id);

    if(it == disks.end()) {
        throw DiskManagerError(DiskManagerErrorCode::DISK_NOT_FOUND);
    }

    return it->second;
};

DiskPageCursor PosixDiskManager::extend(DiskId disk_id) {
    unsigned int size = disk_size(disk_id);
    assert(size % get_page_size() == 0);
    return (size / get_page_size()) + 1;

}

unsigned int PosixDiskManager::disk_size(DiskId disk_id) {
    auto full_path_fname = get_disk_path(loaded_disk_name(disk_id));

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

}
