#ifndef REGISTER_TEST_H
#define REGISTER_TEST_H

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#define ASSERT(test)                          \
    {                                         \
        if(!(test)) {                         \
            disk_manager->destroy(disk_id); \
        }                                     \
        assert(test);                         \
    }

class RegisterTest {
    public:
    using TestFunction = std::function<void()>;
    static RegisterTest& instance() {
        static RegisterTest registry;
        return registry;
    }

    void register_test(const std::string& name, TestFunction func);
    void run_test(const std::string& name) const;
    void list_tests() const;

    private:
    std::unordered_map<std::string, TestFunction> tests;
};

#define REGISTER_TEST(test_name)                                           \
    namespace {                                                            \
    struct Register##test_name {                                           \
        Register##test_name() {                                            \
            RegisterTest::instance().register_test(#test_name, test_name); \
        }                                                                  \
    } register##test_name;                                                 \
    }

#endif // REGISTER_TEST_H
