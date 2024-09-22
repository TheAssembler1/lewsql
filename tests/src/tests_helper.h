#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

#include <mutex>
#include <string>
#include <atomic>

namespace Tests {

static inline std::mutex init_lock;
static inline bool init = false;

static inline std::mutex cur_file_index_lock;
static inline int cur_file_index = 0;

static inline const std::string test_output_path{"/tmp/test_output.log"};

void test_begin();
std::string get_next_random_string(const std::string& prefix);

namespace DiskManagerOpts {
    static inline const std::string temp_storage_dir_path{"/tmp"};
    static inline const std::string disk_prefix{"disk_manager_test_"};
    static inline constexpr unsigned int page_size{512};
    static inline constexpr unsigned int max_disk_size{page_size * 4};
};

}

#endif // TESTS_HELPER_H
