#include <iostream>
#include <buffer_manager.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <memory>
#include <register_test.h>
#include <buffer_page_tracker/bitmap_tracker.h>.h>

#define PAGE_SIZE 512
#define NUM_PAGES 8

/*std::shared_ptr<DiskManager> disk_manager,
std::unique_ptr<ReplacementAlg> replacement_alg,
std::unique_ptr<BufferPageTracker> buffer_page_tracker,
unsigned int num_pages):*/



void pinning_test() {
    auto disk_manager = std::make_shared<PosixDiskManager>("/tmp", PAGE_SIZE);

    DiskId disk_id;
    try {
        disk_id = disk_manager->d_create();

        auto buf_manager = std::make_shared<BufferManager>(
            disk_manager,
            std::make_unique<DumbAlg>(),
            std::make_unique<BitmapTracker>(NUM_PAGES),
            NUM_PAGES
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
