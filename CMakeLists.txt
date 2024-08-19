cmake_minimum_required(VERSION 3.15...3.30)

project(lewsql VERSION 1.0 LANGUAGES CXX)

add_subdirectory(buffer_manager)
add_subdirectory(driver)

add_custom_target(run
    COMMAND driver
    DEPENDS driver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running the driver executable"
)
