#include <iostream>
#include <logger.h>
#include <register_test.h>
#include <string>
#include <ostream>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        RegisterTest::instance().list_tests();
        return 1;
    }

    std::string test_name = argv[1];
    RegisterTest::instance().run_test(test_name);

    return 0;
}
