#include <buffer_manager.h>
#include <buffer_page.h>
#include <buffer_page_tracker/bitmap_tracker.h>
#include <disk_manager.h>
#include <disk_manager_types.h>
#include <impl/posix_disk_manager.h>
#include <logger.h>
#include <register_test.h>
#include <replacement/dumb_alg.h>
#include <stdint.h>

#include "constants.h"

struct TestStruct {
    int32_t field1;
    int32_t field2;
};

void buffer_page_test() {
    // NOTE: storing 12 buffer pages
    uint8_t* buffer = new uint8_t[BufferPage::size(PAGE_SIZE) * NUM_BUFFER_PAGES];

    BufferPage* buffer_page = reinterpret_cast<BufferPage*>(buffer);
    buffer_page->init(PAGE_SIZE, 0, 0, 0, 0, false);

    // NOTE: test writing single struct
    TestStruct test_struct{1234, 4321};
    buffer_page->to_ptr<TestStruct>(0)[0] = test_struct;
    TestStruct* test_struct_ptr = buffer_page->to_ptr<TestStruct>(0);

    // NOTE: sanity check on writing to some of the fields of the buffer page
    assert(buffer_page->initialized);
    assert(buffer_page->page_size == PAGE_SIZE);

    // NOTE: sanity check on writing to the byte page
    assert(test_struct_ptr->field1 == 1234);
    assert(test_struct_ptr->field2 == 4321);

    // NOTE: try to fill buffer pages
    std::vector<TestStruct> test_structs{};
    for(int i = 0; i < NUM_BUFFER_PAGES; i++) {
        test_structs.push_back({i, -i});
        buffer_page->to_ptr<TestStruct>(0)[i] = test_structs[i];
    }

    for(int i = 0; i < NUM_BUFFER_PAGES; i++) {
        assert(buffer_page->to_ptr<TestStruct>(0)[i].field1 == i);
        assert(buffer_page->to_ptr<TestStruct>(0)[i].field2 == -i);
    }

    // NOTE: check memcpy
    for(auto& ts : test_structs) {
        ts.field1 = 5;
        ts.field2 = -5;
    }

    buffer_page->memcpy(0, test_structs.data(), test_structs.size());

    // NOTE: ensure we can read back memcpy
    for(int i = 0; i < NUM_BUFFER_PAGES; i++) {
        assert(buffer_page->to_ptr<TestStruct>(0)[i].field1 == 5);
        assert(buffer_page->to_ptr<TestStruct>(0)[i].field2 == -5);
    }

    test_structs.clear();
    delete[] buffer;
}
REGISTER_TEST(buffer_page_test);
