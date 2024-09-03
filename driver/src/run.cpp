#include <buffer_manager.h>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager_error.h>
#include <organizer.h>
#include <heap.h>

#include <iostream>
#include <vector>

#define PAGE_SIZE 512
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_DISK_NAME "driver.disk"
#define TEST_TABLE_NAME "test.table"

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

    uint8_t test_val = 69;
    TupleCols cols = TupleCols{TypeList::UINT8_T};
    TupleVals vals = TupleVals{&test_val};

    Heap heap{posix_dmanager, buf_manager, TEST_TABLE_NAME, cols, PAGE_SIZE};

    std::cout << "writing tuple" << std::endl;
    heap.push_back_record(Tuple(cols, vals, PAGE_SIZE));

    // NOTE flushing all pages
    std::cout << "flushing pages which should have tuple" << std::endl;
    buf_manager->free_avail_pages();

    // FIXME: maybe this should happen in the heap class itself
    std::cout << "unloading disk" << std::endl;
    posix_dmanager->d_unload(disk_id);

    std::cout << *(dynamic_cast<PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    return 0;
}
