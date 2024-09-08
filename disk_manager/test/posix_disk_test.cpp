#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <iostream>
#include <memory>
#include <register_test.h>

#define PAGE_SIZE 512
#define TEST_DISK_NAME_PREFIX "disk_test"
#define TEST_DISK_NAME_SUFFIX ".disk"
#define TEST_DISK_NAME TEST_DISK_NAME_PREFIX TEST_DISK_NAME_SUFFIX
#define NUM_DISKS_TO_TEST 32

void disk_test() {
    auto disk_manager = std::make_shared<DiskManager::PosixDiskManager>("/tmp");

    DiskId disk_id = disk_manager->d_create(TEST_DISK_NAME);
    ASSERT(disk_manager->num_loaded_disk() == 1);

    disk_manager->d_destroy(disk_id);
    ASSERT(disk_manager->num_loaded_disk() == 0);

    std::vector<DiskId> loaded_disks{};
    for(int i = 0; i < NUM_DISKS_TO_TEST; i++) {
        auto disk_name = TEST_DISK_NAME_PREFIX + std::to_string(i) + TEST_DISK_NAME_SUFFIX;
        loaded_disks.push_back(disk_manager->d_create(disk_name));
    }

    ASSERT(disk_manager->num_loaded_disk() == NUM_DISKS_TO_TEST);

    for(int i = 0; i < NUM_DISKS_TO_TEST; i++) {
        disk_manager->d_destroy(loaded_disks[i]);
    }

    ASSERT(disk_manager->num_loaded_disk() == 0);
}
REGISTER_TEST(disk_test);
