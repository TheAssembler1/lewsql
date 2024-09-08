#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_error.h>
#include <heap.h>
#include <impl/posix_disk_manager.h>
#include <organizer.h>
#include <replacement/dumb_alg.h>
#include <types/uint16_t_type.h>
#include <types/uint32_t_type.h>
#include <types/uint8_t_type.h>
#include <logger.h>

#include <iostream>
#include <vector>

#define PAGE_SIZE (512)
#define MAX_PAGE_SIZE (PAGE_SIZE * 12)
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_DISK_NAME "driver.disk"
#define TEST_TABLE_NAME "test.table"

int main() {
    LOG(LogLevel::TRACE) << "test trace message" << std::endl;
    LOG(LogLevel::INFO) << "test info message" << std::endl;
    LOG(LogLevel::WARNING) << "test warning message" << std::endl;
    LOG(LogLevel::ERROR) << "test error message" << std::endl;
    LOG(LogLevel::FATAL) << "test fatal message" << std::endl;

    return 0;

    std::shared_ptr<DiskManager::DiskManager> posix_dmanager{new DiskManager::PosixDiskManager("/home/ta1/src/test_dir", PAGE_SIZE, MAX_PAGE_SIZE)};
    std::cout << "successfully created disk manager" << std::endl;
    DiskId disk_id;

    try {
        disk_id = posix_dmanager->create(TEST_DISK_NAME);
    } catch(DiskManagerError& err) {
        assert(err.error_code == DiskManagerErrorCode::DISK_ALREADY_EXISTS);
        std::cout << "disk already exists" << std::endl;
        disk_id = posix_dmanager->load(TEST_DISK_NAME);
    }

    std::cout << *(dynamic_cast<DiskManager::PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    std::shared_ptr<ReplacementAlg> replacment_alg{new DumbAlg};
    auto buf_manager = std::make_shared<BufferManager>(
    posix_dmanager, std::make_unique<DumbAlg>(), std::make_unique<BitmapTracker>(PAGE_SIZE), NUM_PAGES, PAGE_SIZE);

    TupleCols cols = TupleCols{TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T};
    Heap heap{posix_dmanager, buf_manager, TEST_TABLE_NAME, cols, PAGE_SIZE};

    /*for(int i = 0; i < 16; i++) {
        std::cout << "writing tuple" << std::endl;

        TupleVals tuple_vals{};
        tuple_vals.emplace_back(std::make_unique<Uint8TType>(1));
        tuple_vals.emplace_back(std::make_unique<Uint8TType>(2));
        tuple_vals.emplace_back(std::make_unique<Uint8TType>(3));
        tuple_vals.emplace_back(std::make_unique<Uint8TType>(4));

        Tuple tuple{std::move(tuple_vals)};
        heap.insert_tuple(std::move(tuple));

        tuple_vals.clear();
    }*/

    /*TupleVals tuple_vals{};
    tuple_vals.emplace_back(std::make_unique<Uint8TType>(4));

    Tuple tuple{std::move(tuple_vals)};
    heap.insert_tuple(std::move(tuple));

    tuple_vals.clear();*/

    // NOTE flushing all pages
    std::cout << "flushing pages which should have tuple" << std::endl;
    buf_manager->free_avail_pages();

    // FIXME: maybe this should happen in the heap class itself
    std::cout << "unloading disk" << std::endl;
    posix_dmanager->unload(disk_id);

    std::cout << *(dynamic_cast<DiskManager::PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    return 0;
}
