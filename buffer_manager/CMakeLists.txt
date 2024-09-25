cmake_minimum_required(VERSION 3.15...3.30)

project(buffer_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/buffer_manager.cpp
    src/replacement/dumb_alg.cpp
    src/memory_pool.cpp
    src/buffer_page_tracker/bitmap_tracker.cpp
    src/mem/buffer_page_allocator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE library disk_manager)
target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC src 
)
