#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

#include <mutex>
#include <string>

namespace Tests {

static inline std::mutex init_lock;
static inline bool init = false;

struct DiskManagerOpts {
    static inline const std::string temp_storage_dir_path{"/tmp"};
    static inline constexpr unsigned int page_size{512};
    static inline constexpr unsigned int num_pages{8};
    static inline constexpr unsigned int max_disk_size{page_size * 4};
};

void test_begin();

}

#endif // TESTS_HELPER_H
