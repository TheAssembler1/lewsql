cmake_minimum_required(VERSION 3.15...3.30)

project(driver VERSION 1.0 LANGUAGES CXX)

set(SOURCES src/run.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE buffer_manager)
target_link_libraries(${PROJECT_NAME} PRIVATE disk_manager)
