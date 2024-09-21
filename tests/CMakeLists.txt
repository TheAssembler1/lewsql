cmake_minimum_required(VERSION 3.15...3.30)

project(tests VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/entry.cpp
    src/tests_helper.cpp
    src/disk_manager/posix_disk_manager_test.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE library organizers buffer_manager disk_manager)
