cmake_minimum_required(VERSION 3.15...3.30)

project(driver VERSION 1.0 LANGUAGES CXX)

set(SOURCES src/run.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE library organizers buffer_manager disk_manager compiler)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_DIR="${CMAKE_SOURCE_DIR}")
