cmake_minimum_required(VERSION 3.15...3.30)

project(library VERSION 1.0 LANGUAGES CXX)

set(SOURCES
    src/bitmap.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC src
)


set(LIBRARY_TEST_EXEC library_test)
set(TEST_SOURCES
    test/bitmap_test.cpp
    test/result_test.cpp
)

add_executable(${LIBRARY_TEST_EXEC} "test/library_test.cpp" ${TEST_SOURCES})
target_link_libraries(${LIBRARY_TEST_EXEC} PRIVATE test_helper library)

foreach(test ${TEST_SOURCES})
  get_filename_component(TName ${test} NAME_WE)
  add_test(NAME ${TName} COMMAND ${LIBRARY_TEST_EXEC} ${TName})
endforeach()
