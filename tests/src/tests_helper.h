#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

#include <mutex>
#include <string>

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
    inline constexpr unsigned int page_byte_size{512};
    inline constexpr unsigned int max_disk_num_pages{4};
    inline constexpr unsigned int max_disk_byte_size{page_byte_size * max_disk_num_pages};
};

}

#endif // TESTS_HELPER_H
