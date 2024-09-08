cmake_minimum_required(VERSION 3.15...3.30)

project(disk_manager VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/impl/posix_disk_manager.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE library)

target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC src 
)

set(DISK_MANAGER_TEST_EXEC disk_manager_manager_test)
set(TEST_SOURCES
    test/posix_disk_test.cpp
)

add_executable(${DISK_MANAGER_TEST_EXEC} "test/disk_manager_test.cpp" ${TEST_SOURCES})
target_link_libraries(${DISK_MANAGER_TEST_EXEC} PRIVATE library test_helper disk_manager)

foreach(test ${TEST_SOURCES})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME ${TName} COMMAND ${DISK_MANAGER_TEST_EXEC} ${TName})
endforeach()
