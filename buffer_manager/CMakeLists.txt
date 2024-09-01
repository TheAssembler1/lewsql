cmake_minimum_required(VERSION 3.15...3.30)

project(buffer_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/buffer_manager.cpp
    src/replacement/dumb_alg.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE disk_manager)
target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC src 
)

set(BUFFER_MANAGER_TEST_EXEC buffer_manager_test)
set(TEST_SOURCES
    test/pinning_test.cpp
    test/io_test.cpp
)

add_executable(${BUFFER_MANAGER_TEST_EXEC} "test/buffer_manager_test.cpp" ${TEST_SOURCES})
target_link_libraries(${BUFFER_MANAGER_TEST_EXEC} PRIVATE test_helper buffer_manager disk_manager)

foreach(test ${TEST_SOURCES})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME ${TName} COMMAND ${BUFFER_MANAGER_TEST_EXEC} ${TName})
endforeach()
