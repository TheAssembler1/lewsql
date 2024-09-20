#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <iostream>
#include <memory>
#include <gtest/gtest.h>

#define PAGE_SIZE (512)
#define MAX_DISK_SIZE (PAGE_SIZE * 12)
#define TEST_DISK_NAME_PREFIX "disk_test"
#define TEST_DISK_NAME_SUFFIX ".disk"
#define TEST_DISK_NAME TEST_DISK_NAME_PREFIX TEST_DISK_NAME_SUFFIX
#define NUM_DISKS_TO_TEST (32)

TEST(posix_disk_test, BasicAssertions) {
    std::ostream* test = &std::cerr;
    Logger::init({nullptr});
    //auto disk_manager = std::make_shared<DiskManager::PosixDiskManager>("/tmp", PAGE_SIZE, MAX_DISK_SIZE);

    //auto disk_id = disk_manager->create(TEST_DISK_NAME).get_value();

    /*EXPECT_TRUE(disk_manager->num_loaded_disks().get_value() == 1);

    disk_manager->destroy(disk_id);
    EXPECT_TRUE(disk_manager->num_loaded_disks().get_value() == 0);

    std::vector<DiskId> loaded_disks{};
    for(int i = 0; i < NUM_DISKS_TO_TEST; i++) {
        auto disk_name = TEST_DISK_NAME_PREFIX + std::to_string(i) + TEST_DISK_NAME_SUFFIX;
        loaded_disks.push_back(disk_manager->create(disk_name).get_value());
    }

    EXPECT_TRUE(disk_manager->num_loaded_disks().get_value() == NUM_DISKS_TO_TEST);

    for(int i = 0; i < NUM_DISKS_TO_TEST; i++) {
        disk_manager->destroy(loaded_disks[i]);
    }

    EXPECT_TRUE(disk_manager->num_loaded_disks().get_value() == 0);*/
}
