#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <iostream>
#include <logger.h>
#include <memory>
#include <register_test.h>
#include <replacement/dumb_alg.h>

#include "constants.h"

#define TEST_DISK_NAME "memory_pool_test.disk"

void memory_pool_test() {
    auto disk_manager = std::make_shared<DiskManager::PosixDiskManager>("/tmp", PAGE_SIZE, MAX_DISK_SIZE);
    auto alg = std::make_shared<DumbAlg>();

    DiskId disk_id;
    try {
        disk_id = disk_manager->create(TEST_DISK_NAME).value_or([&](DiskManagerError error) {
            return static_cast<DiskId>(disk_manager->load(TEST_DISK_NAME).get_value());
        });

        auto buf_manager = std::make_shared<BufferManager>(disk_manager, std::make_unique<DumbAlg>(),
        std::make_unique<BitmapTracker>(NUM_BUFFER_PAGES), NUM_BUFFER_PAGES, PAGE_SIZE);


        for(int i = 0; i < NUM_BUFFER_PAGES; i++) {
            BufferPage& page = buf_manager->pin(disk_id, i);
            for(int j = 0; j < PAGE_SIZE; j++) {
                LOG(LogLevel::INFO) << "testing (page, byte) = (" << i << ", " << j << ")" << std::endl;
                assert(page.bytes[j] == 0);
            }
        }
    } catch(std::exception& e) {
        LOG(LogLevel::ERROR) << e.what() << std::endl;
        disk_manager->destroy(disk_id);
        throw e;
    }

    disk_manager->destroy(disk_id);
}
REGISTER_TEST(memory_pool_test);
