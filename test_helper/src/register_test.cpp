#include "register_test.h"

void RegisterTest::register_test(const std::string& name, TestFunction func) {
    tests[name] = func;
}

void RegisterTest::run_test(const std::string& name) const {
    auto it = tests.find(name);
    if(it != tests.end()) {
        std::cout << "Running test: " << name << std::endl;
        it->second();
    } else {
        std::cerr << "Unknown test: " << name << std::endl;
    }
}

void RegisterTest::list_tests() const {
    std::cout << "Available tests:" << std::endl;
    for(const auto& test : tests) {
        std::cout << "  - " << test.first << std::endl;
    }
}
