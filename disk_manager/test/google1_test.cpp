#include <gtest/gtest.h>
#include <logger.h>
#include <ostream>

TEST(HelloTest1, BasicAssertions) {
    std::vector<std::ostream*> test{};
    Logger::init({nullptr});

           //LOG(LogLevel::INFO) << "Hello World" << std::endl;

           //EXPECT_STRNE("hello" ,"world");
           //EXPECT_EQ(7 * 6, 42);
}
