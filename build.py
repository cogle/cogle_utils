#!/usr/bin/env python3

import os
import sys
import enum
import json
import argparse
import subprocess
import multiprocessing

from typing import Dict, List, Optional

#Make sure that each key is reflected in on of the below lists
COMPILER_FLAG_KEY = "compiler"
BUILD_FLAG_KEY = "build"
TESTS_FLAG_KEY = "tests"
TSAN_FLAG_KEY = "tsan"
ASAN_FLAG_KEY = "asan"
CLEAN_FLAG_KEY = "clean"

#TODO CLEAN FLAG MAYBE ALSO STORE LAST BUILD IN JSON FILE TO RELOAD
#TODO ALLOW BUILD DIR FLAG TO BE PASSED IN
#TODO PRINT GIT BRANCH at end and start
#TODO SUPPORT COMPILER SWITCHING VIA ENV SETTING
#TODO SUPPORT CORES FLAG

CMAKE_BUILD_ARGS_KEYS_SET = {BUILD_FLAG_KEY, TESTS_FLAG_KEY, TSAN_FLAG_KEY, ASAN_FLAG_KEY}
BUILD_ENV_KEYS_SET = {COMPILER_FLAG_KEY}

TSAN_BUILD = "-DWITH_TSAN=true"
ASAN_BUILD = "-DWITH_ASAN=true"

UNIT_TESTS_BUILD = "-DWITH_TESTS=true"

BUILD_CONFIG = ".build.conf.json"

EXIT_CODE_FAIL = -1

class Parser:
    @staticmethod
    def parse_build_args(args_dict) -> List[str]:
        pass

    @staticmethod
    def parse_cmake_args(args_dict) -> List[str]:
        cmake_args = list()

        for k in args_dict:
            if k in CMAKE_BUILD_ARGS_KEYS_SET:
                cmake_args.append(args_dict[k])            

        return cmake_args

class BuildType(enum.Enum):
    DEBUG = 0
    RELEASE = 1

class CompilerType(enum.Enum):
    GNU = 0
    CLANG = 1

class BuildInfo:
    def __init__(self, compiler: CompilerType, build_type: BuildType, build_dir: str, cmake_flags: List[str], make_flags: List[str]):
        self.compiler = compiler
        self.build_type = build_type

        self.build_dir = build_dir 
        self.cmake_flags = cmake_flags
        self.make_flags = make_flags 

class BuildConfig:
    """
    This is a class that will hold a JSON representation of the build parameters currently
    in use for the project.
    {
        "builds" : {
            "activeBuild" : {
                "compiler" : "gnu",
                "buildDirectory" : "directory",
                "cmakeFlags" : ["flag1", "flag2", ....],
                "makeFlags" : ["flag1", "flag2:, ...]
            },
            "debugBuild" : {
                "compiler" : "clang",
                "buildDirectory" : "directory",
                "cmakeFlags" : ["flag1", "flag2", ....],
                "makeFlags" : ["flag1", "flag2:, ...]
            },
            "releaseBuild" : {
                "compiler" : "gnu",
                "buildDirectory" : "directory",
                "cmakeFlags" : ["flag1", "flag2", ....],
                "makeFlags" : ["flag1", "flag2:, ...]
            }
        }
    }
    """

    ROOT_OBJ_KEY      = "buildInfo"
    ACTIVE_BUILD_KEY  = "activeBuild"
    DEBUG_BUILD_KEY   = "debugBuild"
    RELEASE_BUILD_KEY = "releaseBuild"

    COMPILER_KEY        = "compiler"
    BUILD_DIRECTORY_KEY = "buildDirectory"
    CMAKE_FLAGS_KEY     = "makeFlags"
    MAKE_FLAGS_KEY      = "makeFlags"

    def __init__(self, json_obj: str):
        self.json_config = json_obj
    
    #def update_active(self, build_type, build_dir, cmake_flags, make_flags):
    #    pass

    def has_active(self) -> bool:
        if not self.json_config:
            return False

        if self.ROOT_OBJ_KEY in self.json_config and self.ACTIVE_BUILD_KEY in self.json_config:
           return True

        return False


def create_build_dir(args_dict) -> str:
    return "{}_{}_build".format(args_dict[COMPILER_FLAG_KEY].name, args_dict[BUILD_FLAG_KEY].name).lower()

def extract_env_args(args_dict):
    #https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    env_dict = dict()

    return env_dict

def load_build_config(build_config_path: str) -> BuildConfig:
    build_config_file = os.path.join(build_config_path, BUILD_CONFIG)

    if os.path.exists(build_config_file):
        with open(BUILD_CONFIG, "r") as f:
            data = json.load(f)
            return BuildConfig(data)
    else:
        return BuildConfig(None)

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

    #If there is nothing then early terminate and attempt to use the cached version
    if sys.argv == 1:
        return ret

    #make clean
    if args.clean:
        # If the clean flag is specified then early return.
        ret[CLEAN_FLAG_KEY] = True
        return ret
    else:
        ret[CLEAN_FLAG_KEY] = False


    #Check and ensure that either GNU or Clang is selected
    if args.gnu and args.clang:
        parser.error("Unable to build with both G++ and Clang++ please specify only ONE")

    #Check and ensure that either Debug or Release is selected.
    if args.debug and args.release:
        parser.error("Unable to build with both G++ and Clang++ please specify only ONE")

    #Assign all variables based upon input parameters

    # Compiler Selection
    if args.gnu:
        ret[COMPILER_FLAG_KEY] = CompilerType.GNU
    elif args.clang:
        #Default compiler is clang++
        ret[COMPILER_FLAG_KEY] = CompilerType.CLANG

    # Build Type Selection
    if args.release:
        ret[BUILD_FLAG_KEY] = BuildType.RELEASE
    elif args.debug:
        ret[BUILD_FLAG_KEY] = BuildType.DEBUG

    #tsan
    if args.tsan:
        ret[TSAN_FLAG_KEY] = TSAN_BUILD

    #asan
    if args.asan:
        ret[ASAN_FLAG_KEY] = ASAN_BUILD

    #unit tests
    if args.tests:
        ret[TESTS_FLAG_KEY] = UNIT_TESTS_BUILD

    return ret

def perform_build(args_dict) -> None:
    project_dir = os.getcwd()

    build_info = setup_build_args(args_dict, project_dir)
    print(f"DEBUG\n{build_info.__dict__}")

    # Create folder(if needed) and go into that directory
    #if not os.path.exists(build_dir):
    #    os.makedirs(build_dir)

    #build_dir = create_build_dir(args_dict)
    #os.chdir(build_dir)



    #run_cmake(os.path.join(cur_dir, "CMakeLists.txt"), cmake_build_commands)
    #print()
    #run_make()


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
    
    os.chdir(project_dir)

def run_cmake(cmake_url: str, cmake_build_commands: List[str]) -> None:
    if not os.path.exists(cmake_url):
        print("Unable to find CMakeLists.txt at location {}".format(cmake_url))
        exit(EXIT_CODE_FAIL)

    #TODO: Fix cmd below via 
    #https://stackoverflow.com/questions/18826789/cmake-output-build-directory
    #-S for source dir 
    #-B for build dir

    print("~~~~~~~~~~CMAKE Config~~~~~~~~~~")
    #cmd = ["cmake", cmake_url] + cmake_build_commands
    print("~~~~~~~~~~CMAKE Config has completed successfully~~~~~~~~~~")

def run_make():
    #    make_command = ["make","-j", "{}".format(max(1, multiprocessing.cpu_count() -2))]
    pass 

def run_subprocess_and_check(cmd: List[str]):
    try:
        subprocess.check_call(cmd)
    except Exception as e:
        print("\n\n")
        print(e)
        print("\n\n")
        print("<-----------------------------------Exception occurred when running----------------------------------->")
        print(" ".join(cmd))

        exit(EXIT_CODE_FAIL)

def save_build_config(build_info):
    pass

def setup_build_args(args_dict, project_dir: str) -> BuildInfo:
    previous_build = load_build_config(project_dir)

    #Cases
    #1) There is no active build set the incoming as the active build 
    #2) The CMake Commands from the args_dict are empty(This by default means last build) and there is an active build.
    #3) The only key in the args_dict is make.
    #4) This build is a new build set it to the appropriate build type and use this.

    build_info: BuildInfo = None    

    if not previous_build.has_active():
        # Case 1
        print("No active build detected")
        #build_info = BuildInfo(args_dict[COMPILER_FLAG_KEY], args_dict[BUILD_FLAG_KEY], "", [], [])
    elif len(args_dict) == 0:
        # Case 2
        print("No using cached build")
    elif len(args_dict) == 1 and CLEAN_FLAG_KEY in args_dict:
        # Case 3
        print("Using cached build and cleaning")
    else:
        # Case 4
        pass

    #TODO Handle the case where the build is different then the currently active build
    cmake_build_commands = Parser.parse_cmake_args(args_dict)
    env_args = extract_env_args(args_dict)

    return build_info



if __name__ == "__main__":
    perform_build(parse_args())