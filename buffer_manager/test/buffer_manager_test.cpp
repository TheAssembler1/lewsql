#include <iostream>
#include <register_test.h>
#include <string>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <test_name>\n";
        RegisterTest::instance().list_tests();
        return 1;
    }

    std::string test_name = argv[1];
    RegisterTest::instance().run_test(test_name);

    return 0;
}
