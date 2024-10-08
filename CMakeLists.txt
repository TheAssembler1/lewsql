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
add_subdirectory(compiler)
add_subdirectory(driver)
add_subdirectory(tests)

add_custom_target(run
    COMMAND driver
    DEPENDS driver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running the driver executable"
)
