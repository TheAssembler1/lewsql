//#feature on safety
#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_error.h>
#include <heap.h>
#include <impl/posix_disk_manager.h>
#include <logger.h>
#include <organizer.h>
#include <replacement/dumb_alg.h>
#include <types/uint16_t_type.h>
#include <types/uint32_t_type.h>
#include <types/uint8_t_type.h>
%:include <mem/buffer_page_allocator.h>

#define OUTPUT_LOG_FILE "output.log"
#define PAGE_SIZE (512)
#define MAX_PAGE_SIZE (PAGE_SIZE * 12)
#define NUM_PAGES 8
#define DISK_CURSOR 0
#define TEST_HEAP_TABLE_NAME "test"

int main() {
    Logger::init({&std::cerr});

    LOG(LogLevel::INFO) << "Hello World" << std::endl;

    return 0;
}
