#include <buffer_manager.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <buffer_page.h>
#include <impl/posix_disk_manager.h>
#include <register_test.h>
#include <replacement/dumb_alg.h>
#include <logger.h>

#include "constants.h"

void buffer_page_test() {
    uint8_t* buffer = new uint8_t[PAGE_SIZE];

    delete[] buffer;
}
REGISTER_TEST(buffer_page_test);
