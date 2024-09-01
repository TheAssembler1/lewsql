cmake_minimum_required(VERSION 3.15...3.30)

project(test_helper VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/register_test.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC src
)
