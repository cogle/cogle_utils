cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils)

# Move to cmake modules and read more on how todo properly
set(CMAKE_CXX_STANDARD 17)
set(CUSTOM_COMPILER_FLAGS "")

if (WITH_TSAN) 
    message(STATUS "Building with TSAN")
endif(WITH_TSAN)

if (WITH_ASAN)
    message(STATUS "Building with ASAN")
    set(CUSTOM_COMPILER_FLAGS "${CUSTOM_COMPILER_FLAGS} -fsanitize=address")
endif(WITH_ASAN)


if (WITH_TESTS)
    message(STATUS "Building and executing tests")
    # Enable testing
    enable_testing()

    set(TEST_TARGET "utils_unit_tests")
    set(TEST_SOURCES
        tests/main.cpp
    )

    add_executable(${TEST_TARGET} ${TEST_SOURCES})

    target_include_directories(
        ${TEST_TARGET}
        PRIVATE
        third_party/
    )

    add_test(${TEST_TARGET}, ${TEST_TARGET})
endif(WITH_TESTS)