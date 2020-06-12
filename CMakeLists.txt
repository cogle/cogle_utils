cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils)

set(CMAKE_CXX_STANDARD 20)
set(CUSTOM_COMPILER_FLAGS "")

if (WITH_TSAN) 
    message(STATUS "Building with TSAN")
endif(WITH_TSAN)

if (WITH_ASAN)
    message(STATUS "Building with ASAN")
    set(CUSTOM_COMPILER_FLAGS "${CUSTOM_COMPILER_FLAGS} -fsanitize=address")
endif(WITH_ASAN)

set(LIB_TARGET cogle_utils)
add_library(${LIB_TARGET} INTERFACE)
add_library(${PROJECT_NAME}::lib ALIAS ${LIB_TARGET})

target_include_directories(${LIB_TARGET} INTERFACE
    include/
)

if (WITH_TESTS)
    message(STATUS "Building and executing tests")
    # Enable testing
    enable_testing()

    set(TEST_TARGET "utils_unit_tests")
    set(TEST_SOURCES
        tests/test_ok.cpp
        tests/main.cpp
    )

    add_executable(${TEST_TARGET} ${TEST_SOURCES})

    target_include_directories(
        ${TEST_TARGET}
        PRIVATE
        include/
        third_party/
    )

    add_test(${TEST_TARGET}, ${TEST_TARGET})
endif(WITH_TESTS)