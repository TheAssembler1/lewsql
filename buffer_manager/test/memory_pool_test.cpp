#include <iostream>
#include <buffer_manager.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <memory>
#include <register_test.h>
#include <buffer_page_tracker/bitmap_tracker.h>

#define PAGE_SIZE 512
#define NUM_PAGES 8
void memory_pool_test() {
    auto disk_manager = std::make_shared<PosixDiskManager>("/tmp", PAGE_SIZE);
    auto alg = std::make_shared<DumbAlg>();

    DiskId disk_id;
    try {
        disk_id = disk_manager->d_create();

        auto buf_manager = std::make_shared<BufferManager>(
            disk_manager,
            std::make_unique<DumbAlg>(),
            std::make_unique<BitmapTracker>(NUM_PAGES),
            NUM_PAGES
        );


        for(int i = 0; i < NUM_PAGES; i++) {
            BufferPage* page = buf_manager->pin(disk_id, i);
            for(int j = 0; j < PAGE_SIZE; j++) {
                std::cout << "testing (page, byte) = (" << i << ", " << j << ")" << std::endl;
                assert(page->bytes[j] == 0);
            }
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        disk_manager->d_destroy(disk_id);
        throw e;
    }

    disk_manager->d_destroy(disk_id);
}
REGISTER_TEST(memory_pool_test);
