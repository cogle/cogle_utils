cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils_unit_Tests)

message(STATUS "Building Unit Tests")

set(TEST_TARGET "utils_unit_tests")
set(TEST_SOURCES
    test_result.cpp
    test_ok.cpp
    test_err.cpp
    main.cpp
)

add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_compile_options(${TEST_TARGET} PRIVATE ${CUSTOM_COMPILER_FLAGS})

target_include_directories(
    ${TEST_TARGET}
    PRIVATE
    ${THIRD_PARTY_INCLUDES}
)

target_link_libraries(
    ${TEST_TARGET}
    PUBLIC
    cogle_utils::lib
    ${CUSTOM_LINKER_FLAGS}
)

add_test(NAME UtilsUnitTests COMMAND ${TEST_TARGET} -s)