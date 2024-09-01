#include <iostream>
#include <buffer_manager.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <memory>
#include <register_test.h>

#define PAGE_SIZE 512
#define NUM_PAGES 8

void io_test() {
    auto disk_manager = std::make_shared<PosixDiskManager>("/tmp", PAGE_SIZE);
    auto alg = std::make_shared<DumbAlg>(NUM_PAGES);

    DiskId disk_id;
    try {
        disk_id = disk_manager->d_create();
        ASSERT(disk_id == 0);

        auto buf_manager = std::make_shared<BufferManager>(disk_manager, alg, NUM_PAGES);


    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        disk_manager->d_destroy(disk_id);
        throw e;
    }

    disk_manager->d_destroy(disk_id);
}
REGISTER_TEST(io_test);
