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
#define TEST_DISK_NAME "pinning_test.disk"

void pinning_test() {
    auto disk_manager = std::make_shared<PosixDiskManager>("/tmp");

    DiskId disk_id;
    try {
        disk_id = disk_manager->d_create(TEST_DISK_NAME);

        auto buf_manager = std::make_shared<BufferManager>(
            disk_manager,
            std::make_unique<DumbAlg>(),
            std::make_unique<BitmapTracker>(NUM_PAGES),
            NUM_PAGES,
            PAGE_SIZE
        );

        ASSERT(buf_manager->buffer_page_tracker->get_num_free_pages() == NUM_PAGES);
        ASSERT(buf_manager->buffer_page_tracker->get_num_taken_pages() == 0);

        for(int i = 0; i < NUM_PAGES; i++) {
            buf_manager->pin(disk_id, i);
        }

        ASSERT(buf_manager->buffer_page_tracker->get_num_free_pages() == 0);
        ASSERT(buf_manager->buffer_page_tracker->get_num_taken_pages() == NUM_PAGES);

        for(int i = 0; i < NUM_PAGES; i++) {
            buf_manager->unpin(disk_id, i);
        }

        ASSERT(buf_manager->buffer_page_tracker->get_num_free_pages() == 0);
        ASSERT(buf_manager->buffer_page_tracker->get_num_taken_pages() == NUM_PAGES);

        buf_manager->free_avail_pages();

        ASSERT(buf_manager->buffer_page_tracker->get_num_free_pages() == NUM_PAGES);
        ASSERT(buf_manager->buffer_page_tracker->get_num_taken_pages() == 0);

        disk_manager->d_destroy(disk_id);
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        disk_manager->d_destroy(disk_id);
        throw e;
    }
}
REGISTER_TEST(pinning_test);
