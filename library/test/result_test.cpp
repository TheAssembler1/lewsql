#include "result.h"

#include <register_test.h>
#include <cassert>
#include <string>

class TestClass {

};

class NoCopyTestClass {
    public:
        NoCopyTestClass() = default;
        NoCopyTestClass(NoCopyTestClass&&) = default; // Movable
        NoCopyTestClass& operator=(NoCopyTestClass&&) = default; // Movable
        NoCopyTestClass(const NoCopyTestClass&) = delete; // Non-copyable
        NoCopyTestClass& operator=(const NoCopyTestClass&) = delete; // Non-copyable
};


void result_test() {
    Result<TestClass, std::exception> res_test_class(TestClass{});
    Result<NoCopyTestClass, std::exception>{NoCopyTestClass{}};

    assert(!res_test_class.is_error());
}
REGISTER_TEST(result_test);
