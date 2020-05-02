#!/usr/bin/env python3

import os
import argparse
import subprocess

COMPILER_FLAG_KEY = "compiler"
BUILD_FLAG_KEY = "build"
TESTS_FLAG_KEY = "tests"
TSAN_FLAG_KEY = "tsan"
ASAN_FLAG_KEY = "asan"

GNU_COMPILER_FLAG_SELECT_VAL   = "gnu"
CLANG_COMPILER_FLAG_SELECT_VAL = "clang"

CMAKE_DEBUG_BUILD = "-DCMAKE_BUILD_TYPE=Debug"
CMAKE_RELEASE_BUILD = "-DCMAKE_BUILD_TYPE=Release"

TSAN_BUILD = "-DWITH_TSAN"
ASAN_BUILD = "-DWITH_ASAN"

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang", help="Use clang++ compiler to build(default)", action="store_true")
    parser.add_argument("--gnu", help="Use g++ compiler to build", action="store_true")
    parser.add_argument("--tsan", help="Build using tsan utility to check thread safety", action="store_true")
    parser.add_argument("--asan", help="Build using asan utility to check memory safety", action="store_true")
    parser.add_argument("--debug", help="Build using debug version", action="store_true")
    parser.add_argument("--release", help="Build using release version", action="store_true")
    args = parser.parse_args()

    ret = dict()

    #Check all parser arg errors/validity below

    #Check and ensure that either GNU or Clang is selected
    if args.gnu and args.clang:
        parser.error("Unable to build with both G++ and Clang++ please specify only ONE")

    #Check and ensure that either Debug or Release is selected.
    if args.debug and args.release:
        parser.error("Unable to build with both G++ and Clang++ please specify only ONE")

    #Assign all variables based upon input parameters

    # Compiler Selection
    if args.gnu:
        ret[COMPILER_FLAG_KEY] = GNU_COMPILER_FLAG_SELECT_VAL
    else:
        #Default compiler is clang++
        ret[COMPILER_FLAG_KEY] = CLANG_COMPILER_FLAG_SELECT_VAL

    # Build Type Selection
    if args.release:
        ret[BUILD_FLAG_KEY] = CMAKE_RELEASE_BUILD
    else:
        #Default build option is Debug
        ret[BUILD_FLAG_KEY] = CMAKE_DEBUG_BUILD

    #tsan
    if args.tsan:
        ret[TSAN_FLAG_KEY] = TSAN_BUILD

    if args.asan:
        ret[ASAN_FLAG_KEY] = ASAN_BUILD

    return ret

def create_build_dir(args_dict):
    build_str = ""
    if args_dict[BUILD_FLAG_KEY] == CMAKE_DEBUG_BUILD:
        build_str = "debug"
    elif args_dict[BUILD_FLAG_KEY] == CMAKE_DEBUG_RELEASE:
        build_str = "release"
    else:
        raise ValueError("Key {} does not belong".format(args_dict[BUILD_FLAG_KEY]))

    return "{}_{}_build".format(args_dict[COMPILER_FLAG_KEY], build_str)

def extract_cmake_args(args_dict):
    pass

def extract_env_args(args_dict):
    pass

def perform_build(args_dict):
    #https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake

    # Create folder(if needed) and go into that directory
    # FIRST TODO SAVE CURRENT DIRECTORY TO POP UP AND RETURN
    build_dir = create_build_dir(args_dict)

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    os.chdir(build_dir)
    

if __name__ == "__main__":
    perform_build(parse_args())