#include "../unit_test_framework.hpp"
#include "../tests_helper.h"

#include <iostream>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>

using namespace Tests;

TEST(posix_disk_manager_create) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};
}

TEST(posix_disk_manager_create_disk) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};

    auto disk_id_res = posix_disk.create(Tests::get_next_random_string(Tests::DiskManagerOpts::disk_prefix));
    ASSERT_TRUE(disk_id_res.is_value());
}
