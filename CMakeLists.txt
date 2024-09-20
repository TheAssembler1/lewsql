cmake_minimum_required(VERSION 3.15...3.30)

project(lewsql VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(PROJECT_SOURCE_DIR PUBLIC ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(SAFE_CXX_BUILD_TESTING ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(library)
add_subdirectory(disk_manager)
add_subdirectory(buffer_manager)
add_subdirectory(organizers)
add_subdirectory(driver)

add_custom_target(run
    COMMAND driver
    DEPENDS driver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running the driver executable"
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(google_test
    disk_manager/test/google_test.cpp
    disk_manager/test/google1_test.cpp
    #disk_manager/test/posix_disk_test.cpp
)
target_link_libraries(google_test GTest::gtest_main library)

include(GoogleTest)
gtest_discover_tests(google_test)
