#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

#include <mutex>
#include <string>
#include <atomic>

namespace Tests {

inline std::mutex init_lock;
inline bool init = false;

inline std::mutex cur_file_index_lock;
inline int cur_file_index = 0;

void test_begin();
std::string get_next_random_string(const std::string& prefix);

namespace DiskManagerOpts {
    inline const std::string temp_storage_dir_path{"/tmp"};
    inline const std::string disk_prefix{"disk_manager_test_"};
    inline constexpr unsigned int page_size{512};
    inline constexpr unsigned int max_disk_size{page_size * 4};
};

}

#endif // TESTS_HELPER_H
