cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils_examples)

message(STATUS "Building all examples")
set(EXAMPLES_DIR_PATH "${CMAKE_CURRENT_BINARY_DIR}/files")
message(STATUS "Examples will be using ${EXAMPLES_DIR_PATH} as their directory path")

file(REMOVE ${EXAMPLES_DIR_PATH})
file(MAKE_DIRECTORY ${EXAMPLES_DIR_PATH})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/example_helpers.hxx.in 
               ${CMAKE_CURRENT_SOURCE_DIR}/include/example_helpers.hxx 
               @ONLY)

include_directories(include/)

add_subdirectory(posix_result_example)
add_subdirectory(filesystem_result_example)
