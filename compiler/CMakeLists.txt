cmake_minimum_required(VERSION 3.15...3.30)

project(compiler VERSION 1.0 LANGUAGES CXX)

set(SOURCES
        src/lexer/lexer.cpp
        src/char_stream/char_stream.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE library)
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC src
)