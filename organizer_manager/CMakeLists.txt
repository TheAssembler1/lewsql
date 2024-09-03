cmake_minimum_required(VERSION 3.15...3.30)

project(organizer_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/heap.cpp
    src/types/uint8_t_type.cpp
    src/tuple.cpp
    src/types/type.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE test_helper buffer_manager disk_manager)

target_include_directories(${PROJECT_NAME}
    PUBLIC src
)
