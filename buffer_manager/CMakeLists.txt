cmake_minimum_required(VERSION 3.15...3.30)

project(buffer_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/buffer_manager.cpp
    src/replacement/lru.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE disk_manager)

target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC src 
)
