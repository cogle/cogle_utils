#!/usr/bin/env python3

import os
import json
import argparse
import subprocess
import multiprocessing

#Make sure that each key is reflected in on of the below lists
COMPILER_FLAG_KEY = "compiler"
BUILD_FLAG_KEY = "build"
TESTS_FLAG_KEY = "tests"
TSAN_FLAG_KEY = "tsan"
ASAN_FLAG_KEY = "asan"
CLEAN_FLAG_KEY = "clean"

#TODO CLEAN FLAG MAYBE ALSO STORE LAST BUILD IN JSON FILE TO RELOAD

CMAKE_BUILD_ARGS_KEYS_SET = {BUILD_FLAG_KEY, TESTS_FLAG_KEY, TSAN_FLAG_KEY, ASAN_FLAG_KEY}
BUILD_ENV_KEYS_SET = {COMPILER_FLAG_KEY}

GNU_COMPILER_FLAG_SELECT_VAL   = "gnu"
CLANG_COMPILER_FLAG_SELECT_VAL = "clang"

CMAKE_DEBUG_BUILD = "-DCMAKE_BUILD_TYPE=Debug"
CMAKE_RELEASE_BUILD = "-DCMAKE_BUILD_TYPE=Release"

TSAN_BUILD = "-DWITH_TSAN=true"
ASAN_BUILD = "-DWITH_ASAN=true"

UNIT_TESTS_BUILD = "-DWITH_TESTS=true"

BUILD_CONFIG = ".build.conf.json"

class BuildConfig:
    """
    {
        "buildInfo" : {
            activeBuild : {
                buildDirectory : "directory",
                cmakeFlags : ["flag1", "flag2", ....],
                makeFlags : ["flag1", "flag2:, ...]
            }
            debugBuild : {
                buildDirectory : "directory",
                cmakeFlags : ["flag1", "flag2", ....],
                makeFlags : ["flag1", "flag2:, ...]
            }
            releaseBuild : {
                buildDirectory : "directory",
                cmakeFlags : ["flag1", "flag2", ....],
                makeFlags : ["flag1", "flag2:, ...]
            }
        }
    }
    """
    def __init__(self, json_obj):
        self.__dict__ = json_obj
    
    def update_active(self, build_type, build_dir, cmake_flags, make_flags):
        pass

def load_build_config():
    pass

def save_build_config(build_info):
    pass

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang", help="Use clang++ compiler to build(default)", action="store_true")
    parser.add_argument("--gnu", help="Use g++ compiler to build", action="store_true")
    parser.add_argument("--tsan", help="Build using tsan utility to check thread safety", action="store_true")
    parser.add_argument("--asan", help="Build using asan utility to check memory safety", action="store_true")
    parser.add_argument("--debug", help="Build using debug version", action="store_true")
    parser.add_argument("--release", help="Build using release version", action="store_true")
    parser.add_argument("--tests", help="Build with unit tests", action="store_true")
    parser.add_argument("--clean", help="Build clean", action="store_true")

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

    if args.tests:
        ret[TESTS_FLAG_KEY] = UNIT_TESTS_BUILD

    #if args.clean:
    #    ret[CLEAN_FLAG_KEY] = UNIT_TESTS_BUILD


    return ret

def create_build_dir(args_dict):
    build_str = ""
    if args_dict[BUILD_FLAG_KEY] == CMAKE_DEBUG_BUILD:
        build_str = "debug"
    elif args_dict[BUILD_FLAG_KEY] == CMAKE_RELEASE_BUILD:
        build_str = "release"
    else:
        raise ValueError("Key {} does not belong".format(args_dict[BUILD_FLAG_KEY]))

    return "{}_{}_build".format(args_dict[COMPILER_FLAG_KEY], build_str)

def extract_cmake_args(args_dict):
    cmake_args = list()

    for k in args_dict:
        if k in CMAKE_BUILD_ARGS_KEYS_SET:
            cmake_args.append(args_dict[k])            

    return cmake_args

def extract_env_args(args_dict):
    #https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    env_dict = dict()

    return env_dict

def perform_build(args_dict):
    cur_dir = os.getcwd()

    build_dir = create_build_dir(args_dict)

    # Create folder(if needed) and go into that directory
    needs_cmake = False
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
        needs_cmake = True

    os.chdir(build_dir)

    if needs_cmake:
        #run_cmake()
        pass


    #run_make()

    #if False:
    #    pass
    #else:
    #    cmake_build_commands = extract_cmake_args(args_dict)
    #    cmake_command = ["cmake", ".."] + cmake_build_commands

    #    make_command = ["make","-j", "{}".format(max(1, multiprocessing.cpu_count() -2))]

    #try:
    #    print("~~~~~~~~~~CMAKE Config~~~~~~~~~~")
    #    ret = subprocess.run(cmake_command)
    #    
    #    if ret.returncode != 0:
    #        print("CMake configuration FAILED")
    #        exit(ret.returncode)

    #    print()
    #    print()
    #    print("~~~~~~~~~~CMAKE Config has completed successfully~~~~~~~~~~")
    #except Exception as e:
    #    print("Error occurred will attempting to build inside CMake subprocess {}".format(e))
    #    os.chdir(cur_dir)
    #    exit(-1)

    #try:
    #    print("~~~~~~~~~~Running make~~~~~~~~~~")
    #    ret = subprocess.run(make_command)

    #    if ret.returncode != 0:
    #        print("Building has FAILED")
    #        exit(ret.returncode)

    #    print()
    #    print()
    #    print("~~~~~~~~~~Build has completed successfully~~~~~~~~~~")
    #except Exception as e:
    #    print("Error occurred will attempting to build inside make subprocess {}".format(e))
    #    os.chdir(cur_dir)
    #    exit(-1)
    
    

    os.chdir(cur_dir)


if __name__ == "__main__":
    perform_build(parse_args())