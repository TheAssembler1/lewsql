#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <iostream>
#include <memory>
#include <register_test.h>
#include <replacement/dumb_alg.h>

#define PAGE_SIZE 512
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_DISK_NAME "io_test.disk"

void io_test() {
    auto disk_manager = std::make_shared<PosixDiskManager>("/tmp");
    auto alg = std::make_shared<DumbAlg>();

    DiskId disk_id;
    try {
        disk_id = disk_manager->d_create(TEST_DISK_NAME);
        auto buf_manager = std::make_shared<BufferManager>(
        disk_manager, std::make_unique<DumbAlg>(), std::make_unique<BitmapTracker>(NUM_PAGES), NUM_PAGES, PAGE_SIZE);

        // NOTE: pin page and write to first byte
        BufferPage* page = &buf_manager->pin(disk_id, DISK_CURSOR);

        for(int i = 0; i < PAGE_SIZE; i++) {
            std::cout << "byte index: " << i << std::endl;
            std::cout << "writing byte to page: " << static_cast<int>(i % 255) << std::endl;
            page->bytes[i] = i % 255;
            std::cout << "check writing byte to page: " << static_cast<int>(page->bytes[i]) << std::endl;
            ASSERT(page->bytes[i] == i % 255);
        }

        for(int i = 0; i < PAGE_SIZE; i++) {
            std::cout << "byte index: " << i << std::endl;
            std::cout << "test (expected, received) = (" << i % 255 << ", " << static_cast<int>(page->bytes[i]) << ")" << std::endl;
            ASSERT(page->bytes[i] == i % 255);
        }

        buf_manager->set_dirty(disk_id, DISK_CURSOR);

        // NOTE: unpin page and hopefully it flushes
        buf_manager->unpin(disk_id, DISK_CURSOR);
        buf_manager->remove_page_mem_pool_map(page->buffer_page_cursor);

        // NOTE: repin page and assert first byte was written to
        page = &buf_manager->pin(disk_id, DISK_CURSOR);

        for(int i = 0; i < PAGE_SIZE; i++) {
            std::cout << "byte index: " << i << std::endl;
            std::cout << "test (expected, received) = (" << i % 255 << ", " << static_cast<int>(page->bytes[i]) << ")" << std::endl;
            ASSERT(page->bytes[i] == i % 255);
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        disk_manager->d_destroy(disk_id);
        throw e;
    }

    disk_manager->d_destroy(disk_id);
}
REGISTER_TEST(io_test);
