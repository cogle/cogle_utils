cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils)

set(CMAKE_CXX_STANDARD 17)
set(CUSTOM_COMPILER_FLAGS -Wall -Wextra -Wconversion -Werror)
set(CUSTOM_LINKER_FLAGS "")

###################################
#                                 # 
#     Configuration Options       #
#                                 #
###################################

option(WITH_TSAN     "Build with TSAN" OFF) 
option(WITH_ASAN     "Build with ASAN" OFF)
option(WITH_TESTS    "Build with Unit Tests" OFF)
option(WITH_COVERAGE "Build with Code Coverage" OFF)

message(STATUS "Build WITH_TSAN: " ${WITH_TSAN})
message(STATUS "BUILD WITH_ASAN: " ${WITH_ASAN})
message(STATUS "BUILD WITH_TESTS: " ${WITH_TESTS})
message(STATUS "BUILD WITH_COVERAGE: " ${WITH_COVERAGE})

if(WITH_ASAN)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -fsanitize=address)
    set(CUSTOM_LINKER_FLAGS ${CUSTOM_LINKER_FLAGS} -fsanitize=address)
endif(WITH_ASAN)

if(WITH_COVERAGE)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage)
endif(WITH_COVERAGE)

SET(THIRD_PARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(LIB_TARGET cogle_utils)
add_library(${LIB_TARGET} INTERFACE)
add_library(${PROJECT_NAME}::lib ALIAS ${LIB_TARGET})

target_include_directories(${LIB_TARGET} INTERFACE
    include/
)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests/)
endif(WITH_TESTS)