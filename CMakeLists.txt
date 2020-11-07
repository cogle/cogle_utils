cmake_minimum_required(VERSION 3.13.4)
project(cogle_utils)

set(CMAKE_CXX_STANDARD 17)
set(CUSTOM_COMPILER_FLAGS -Wall -Wextra -Wconversion -Werror -Wshadow -Wundef)
set(CUSTOM_LINKER_FLAGS "")

###################################
#                                 # 
#     Configuration Options       #
#                                 #
###################################

option(WITH_TSAN       "Build with TSAN" OFF) 
option(WITH_ASAN       "Build with ASAN" OFF)
option(WITH_UBSAN      "Build with UBSAN" OFF)
option(WITH_GCOV       "Build with GCOV Code Coverage" OFF)
option(WITH_TESTS      "Build with Unit Tests" OFF)
option(WITH_EXAMPLES   "Build with Examples" OFF)

message(STATUS "Build WITH_TSAN: " ${WITH_TSAN})
message(STATUS "BUILD WITH_ASAN: " ${WITH_ASAN})
message(STATUS "BUILD WITH_UBSAN: " ${WITH_UBSAN})
message(STATUS "BUILD WITH_GCOV: " ${WITH_GCOV})
message(STATUS "BUILD WITH_TESTS: " ${WITH_TESTS})
message(STATUS "BUILD WITH_EXAMPLES: " ${WITH_EXAMPLES})

if (WITH_ASAN AND WITH_TSAN)
    message(FATAL_ERROR "Unable to build both ASAN and TSAN together")
endif(WITH_ASAN AND WITH_TSAN)

if(WITH_ASAN)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -fsanitize=address)
    set(CUSTOM_LINKER_FLAGS ${CUSTOM_LINKER_FLAGS} -fsanitize=address)
endif(WITH_ASAN)

if(WITH_TSAN)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -fsanitize=thread)
    set(CUSTOM_LINKER_FLAGS ${CUSTOM_LINKER_FLAGS} -fsanitize=thread)
endif(WITH_TSAN)

if(WITH_UBSAN)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -fsanitize=undefined)
    set(CUSTOM_LINKER_FLAGS ${CUSTOM_LINKER_FLAGS} -fsanitize=undefined)
endif(WITH_UBSAN)

if(WITH_GCOV)
    set(CUSTOM_COMPILER_FLAGS ${CUSTOM_COMPILER_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage)
    set(CUSTOM_LINKER_FLAGS ${CUSTOM_LINKER_FLAGS} --coverage)
endif(WITH_GCOV)

SET(THIRD_PARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(LIB_TARGET cogle_utils)
add_library(${LIB_TARGET} INTERFACE)
add_library(${PROJECT_NAME}::lib ALIAS ${LIB_TARGET})

target_include_directories(${LIB_TARGET} INTERFACE
    include/
)

if(WITH_EXAMPLES)
    add_subdirectory(examples)
endif(WITH_EXAMPLES)

if (WITH_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/Catch2)
    enable_testing()
    add_subdirectory(tests/)
endif(WITH_TESTS)