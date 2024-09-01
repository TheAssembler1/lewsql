#include <buffer_manager.h>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <buffer_page_tracker/bitmap_tracker.h>

#include <iostream>
#include <vector>

#define PAGE_SIZE 512
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_DISK_NAME "driver.disk"

int main() {

    std::shared_ptr<DiskManager> posix_dmanager{new PosixDiskManager("/home/ta1/src/test_dir")};
    std::cout << "successfully created disk manager" << std::endl;

    auto disk_id = posix_dmanager->d_create(TEST_DISK_NAME);
    std::cout << "successfully created disk" << std::endl;

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

        buf_manager->print_mem_pool_map(std::cout, *buf_manager) << std::endl;
        std::cout << *(dynamic_cast<BitmapTracker*>(buf_manager->buffer_page_tracker.get())) << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    posix_dmanager->d_destroy(disk_id);
    std::cout << "successfully destroyed disk" << std::endl;

    return 0;
}
