cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils_filesystem_result_example)

message(STATUS "Building Filesystem Result Example")

set(EXAMPLE_TARGET "filesystem_result_example")
set(EXAMPLE_SOURCES
    main.cpp
)

add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCES})
target_compile_options(${EXAMPLE_TARGET} PRIVATE ${CUSTOM_COMPILER_FLAGS})

target_include_directories(
    ${EXAMPLE_TARGET}
    PRIVATE
    ${THIRD_PARTY_INCLUDES}
)

target_link_options(${EXAMPLE_TARGET} PRIVATE ${CUSTOM_LINKER_FLAGS})
target_link_libraries(
    ${EXAMPLE_TARGET}
    PUBLIC
    cogle_utils::lib
)

