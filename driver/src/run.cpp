#include <buffer_manager.h>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager_error.h>

#include <iostream>
#include <vector>

#define PAGE_SIZE 512
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_DISK_NAME "driver.disk"

int main() {

    std::shared_ptr<DiskManager> posix_dmanager{new PosixDiskManager("/home/ta1/src/test_dir")};
    std::cout << "successfully created disk manager" << std::endl;
    DiskId disk_id;

    try {
        disk_id = posix_dmanager->d_create(TEST_DISK_NAME);
    } catch(DiskManagerError& err) {
        assert(err.error_code == DiskManagerErrorCode::DISK_ALREADY_EXISTS);
        std::cout << "disk already exists" << std::endl;
        disk_id = posix_dmanager->d_load(TEST_DISK_NAME);
    }

    std::cout << *(dynamic_cast<PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    std::shared_ptr<ReplacementAlg> replacment_alg{new DumbAlg};
    auto buf_manager = std::make_shared<BufferManager>(
        posix_dmanager,
        std::make_unique<DumbAlg>(),
        std::make_unique<BitmapTracker>(PAGE_SIZE),
        NUM_PAGES,
        PAGE_SIZE
    );

    try {
        for(int i = 0; i < NUM_PAGES; i++) {
            buf_manager->pin(disk_id, i);
        }

        std::cout << "printing buffer manager mem pool map" << std::endl;
        buf_manager->print_mem_pool_map(std::cout, *buf_manager) << std::endl;
        std::cout << "printing buffer manager bitmap map" << std::endl;
        std::cout << *(dynamic_cast<BitmapTracker*>(buf_manager->buffer_page_tracker.get())) << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "printing disk manager state" << std::endl;
    posix_dmanager->d_unload(disk_id);

    std::cout << *(dynamic_cast<PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    return 0;
}
