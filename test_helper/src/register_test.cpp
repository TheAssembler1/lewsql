#include "register_test.h"

#include <logger.h>

void RegisterTest::register_test(const std::string& name, TestFunction func) {
    tests[name] = func;
}

void RegisterTest::run_test(const std::string& name) const {
    auto it = tests.find(name);
    if(it != tests.end()) {
        LOG(LogLevel::INFO) << "Running test: " << name << std::endl;
        it->second();
    } else {
        LOG(LogLevel::ERROR) << "Unknown test: " << name << std::endl;
    }
}

void RegisterTest::list_tests() const {
    LOG(LogLevel::INFO) << "Available tests:" << std::endl;
    for(const auto& test : tests) {
        LOG(LogLevel::INFO) << "  - " << test.first << std::endl;
    }
}
