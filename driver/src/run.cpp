#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_error.h>
#include <fstream>
#include <heap.h>
#include <impl/posix_disk_manager.h>
#include <iostream>
#include <logger.h>
#include <organizer.h>
#include <ostream>
#include <replacement/dumb_alg.h>
#include <types/uint16_t_type.h>
#include <types/uint32_t_type.h>
#include <types/uint8_t_type.h>
#include <vector>

#define OUTPUT_LOG_FILE "output.log"
#define PAGE_SIZE (512)
#define MAX_PAGE_SIZE (PAGE_SIZE * 12)
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_HEAP_TABLE_NAME "test"

#if defined(__win32__)
const char PATH_SEPARATOR = '\\';
#elif defined(__unix__)
const char PATH_SEPARATOR = '/';
#endif

int main() {
    // NOTE: defined in CMakeLists.txt
    std::string project_dir{PROJECT_DIR};
    // NOTE: setting up logger
    std::string log_file_name{OUTPUT_LOG_FILE};
    std::string output_path = project_dir + PATH_SEPARATOR + log_file_name;
    auto of_stream = std::ofstream{output_path, std::ios::out};
    Logger::init(OsStreams{&std::cerr, &of_stream});
    LOG(LogLevel::INFO) << "successfully initialized logger" << std::endl;

    // NOTE: setting up disk manager
    std::shared_ptr<DiskManager::DiskManager> posix_dmanager{
    new DiskManager::PosixDiskManager("/home/ta1/src/test_dir", PAGE_SIZE, MAX_PAGE_SIZE)};
    LOG(LogLevel::INFO) << "successfully created disk manager" << std::endl;
    LOG(LogLevel::INFO) << *(dynamic_cast<DiskManager::PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    // NOTE: setting up replacmenet alg and buf manager
    std::shared_ptr<ReplacementAlg> replacment_alg{new DumbAlg};
    auto buf_manager = std::make_shared<BufferManager>(
    posix_dmanager, std::make_unique<DumbAlg>(), std::make_unique<BitmapTracker>(PAGE_SIZE), NUM_PAGES, PAGE_SIZE);

    TupleCols cols = TupleCols{TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T, TypeList::UINT8_T};
    Heap heap{posix_dmanager, buf_manager, TEST_HEAP_TABLE_NAME, cols, PAGE_SIZE};

    /*for(int i = 0; i < 16; i++) {
        LOG(LogLevel::INFO) << "writing tuple" << std::endl;

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
    LOG(LogLevel::INFO) << "flushing pages which should have tuple" << std::endl;
    buf_manager->free_avail_pages();

    LOG(LogLevel::INFO) << *(dynamic_cast<DiskManager::PosixDiskManager*>(posix_dmanager.get())) << std::endl;

    return 0;
}
