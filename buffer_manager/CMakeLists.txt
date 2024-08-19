cmake_minimum_required(VERSION 3.15...3.30)

project(buffer_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES src/bmanager.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
  ${PROJECT_NAME} 
  PRIVATE src 
  PUBLIC src/headers
)
