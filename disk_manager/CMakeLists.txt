cmake_minimum_required(VERSION 3.15...3.30)

project(disk_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES src/impl/posix_dmanager.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC src 
  PUBLIC src/impl
  PUBLIC src/headers
)
