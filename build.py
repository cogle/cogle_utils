#!/usr/bin/env python3

import os
import sys
import enum
import json
import argparse
import subprocess
import logging
import multiprocessing

from typing import Dict, List, Optional

logging.basicConfig(level=logging.DEBUG)

COMPILER_FLAG_KEY = "compiler"
TESTS_FLAG_KEY = "tests"
TSAN_FLAG_KEY = "tsan"
ASAN_FLAG_KEY = "asan"
CLEAN_FLAG_KEY = "clean"

#Key Pairs
BUILD_FLAG_KEY = "build"
CMAKE_BUILD_FLAG_KEY = "cmake_build_flag"

BUILD_DIR_FLAG_KEY = "cmake_build_dir"
BUILD_DIR_CMAKE_FLAG_KEY = "cmake_build_dir_flag"


#TODO CLEAN FLAG MAYBE ALSO STORE LAST BUILD IN JSON FILE TO RELOAD
#TODO SUPPORT COMPILER SWITCHING VIA ENV SETTING
#TODO SUPPORT CORES FLAG
#TODO SUPPORT MULTIPLE BUILDS
#TODO MAKE SOURCE DIR CONFIGURABLE

CMAKE_BUILD_ARGS_KEYS_SET = {CMAKE_BUILD_FLAG_KEY, TESTS_FLAG_KEY, TSAN_FLAG_KEY, ASAN_FLAG_KEY, BUILD_DIR_CMAKE_FLAG_KEY}
BUILD_ENV_KEYS_SET = {COMPILER_FLAG_KEY}

REQUIRED_KEYS = {BUILD_FLAG_KEY, BUILD_DIR_FLAG_KEY, COMPILER_FLAG_KEY}

TSAN_BUILD = "-DWITH_TSAN=true"
ASAN_BUILD = "-DWITH_ASAN=true"

UNIT_TESTS_BUILD = "-DWITH_TESTS=true"

BUILD_CONFIG = ".build.conf.json"

EXIT_CODE_FAIL = -1

DEFAULT_CMAKE_DICT = {}
DEFAULT_BUILD_ENV_DICT = {}

class Parser:
    @staticmethod
    def parse_build_args(args_dict) -> List[str]:
        pass

    @staticmethod
    def parse_cmake_args(args_dict) -> Dict[str, str]:
        cmake_args = dict()

        for k in args_dict:
            if k in CMAKE_BUILD_ARGS_KEYS_SET:
                cmake_args[k] = str(args_dict[k])

        return cmake_args

    @staticmethod
    def parse_build_env(args_dict) -> Dict[str, str]:
        build_env = dict()

        for k in args_dict:
            if k in BUILD_ENV_KEYS_SET:
                build_env[k] = str(args_dict[k])

        return build_env

class BuildType(enum.Enum):
    DEBUG = 0
    RELEASE = 1

class CompilerType(enum.Enum):
    GNU = 0
    CLANG = 1

class BuildInfo:
    def __init__(self, build_dir: str, cmake_flags: Dict[str, str], env_vars: Dict[str, str]):
        self.cmake_flags = cmake_flags
        self.env_vars = env_vars
        self.build_dir = build_dir

        self.is_cached = False

    def get_build_dir(self) -> str:
        return self.build_dir

    def get_cached(self) -> bool:
        return self.is_cached

    def get_cmake_flags(self) -> List[str]:
        return [v for v in self.cmake_flags.values()]

    def get_env_vars(self) -> List[str]:
        return [v for v in self.env_vars.values()]

    def mark_cached(self, is_cached: bool) -> None:
        self.is_cached = is_cached

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

def check_default_args(args_dict):
    """Ensure that the passed in args from the user have the proper
    default arguments set.
    """

    validated_args_dict = args_dict

    for k in REQUIRED_KEYS:
        if k not in args_dict:
            if k == BUILD_FLAG_KEY:
                validated_args_dict[k] = BuildType.DEBUG
                validated_args_dict[CMAKE_BUILD_FLAG_KEY] = f"-DCMAKE_BUILD_TYPE={BuildType.DEBUG.name}"
            elif k == BUILD_DIR_FLAG_KEY:
                build_dir = os.path.join(os.getcwd(), "build")

                validated_args_dict[k] = build_dir
                validated_args_dict[BUILD_DIR_CMAKE_FLAG_KEY] = f"-B{build_dir}"
            elif k == COMPILER_FLAG_KEY:
                validated_args_dict[k] = CompilerType.CLANG
            else:
                print(f"Key {k} is required but is not handled")
                exit(EXIT_CODE_FAIL)
    
    return validated_args_dict

#TODO support multiple cached builds
#def create_build_folder(args_dict) -> str:
#    return "{}_{}_build".format(args_dict[COMPILER_FLAG_KEY].name, args_dict[BUILD_FLAG_KEY].name).lower()

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
    parser.add_argument("--dir", help="Directory to place build folder in", action="store", type=str)
    parser.add_argument("--clang", help="Use clang++ compiler to build(default)", action="store_true")
    parser.add_argument("--gnu", help="Use g++ compiler to build", action="store_true")
    parser.add_argument("--tsan", help="Build using tsan utility to check thread safety", action="store_true")
    parser.add_argument("--asan", help="Build using asan utility to check memory safety", action="store_true")
    parser.add_argument("--debug", help="Build using debug version(default)", action="store_true")
    parser.add_argument("--release", help="Build using release version", action="store_true")
    parser.add_argument("--tests", help="Build with unit tests", action="store_true")
    parser.add_argument("--clean", help="Build clean", action="store_true")

    args = parser.parse_args()

    ret = dict()

    #Extract all parser args below

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
        ret[COMPILER_FLAG_KEY] = CompilerType.CLANG

    # Build Type Selection
    if args.release:
        ret[BUILD_FLAG_KEY] = BuildType.RELEASE
        ret[CMAKE_BUILD_FLAG_KEY] = f"-DCMAKE_BUILD_TYPE={BuildType.RELEASE.name}"
    elif args.debug:
        ret[BUILD_FLAG_KEY] = BuildType.DEBUG
        ret[CMAKE_BUILD_FLAG_KEY] = f"-DCMAKE_BUILD_TYPE={BuildType.DEBUG.name}"

    #tsan
    if args.tsan:
        ret[TSAN_FLAG_KEY] = TSAN_BUILD

    #asan
    if args.asan:
        ret[ASAN_FLAG_KEY] = ASAN_BUILD

    #unit tests
    if args.tests:
        ret[TESTS_FLAG_KEY] = UNIT_TESTS_BUILD
    
    #build dir
    if args.dir:
        if not os.path.exists(args.dir):
            raise parser.error(f"Directory {args.dir} does not exists please create and run again.")

        ret[BUILD_DIR_CMAKE_FLAG_KEY] = f"-B{args.dir}"
        ret[BUILD_DIR_FLAG_KEY] = f"{args.dir}"

    return ret

def perform_build(args_dict) -> None:
    project_dir = os.getcwd()

    run_git_info()

    build_info = setup_build_args(args_dict, project_dir)
    logging.debug(f"{build_info.__dict__}")

    build_dir = build_info.get_build_dir()

    if not build_info.get_cached() and not os.path.exists(build_dir):
        print(f"Creating {build_dir}")
        os.mkdir(build_dir)

    if build_info.get_cached() and not os.path.exists(build_dir):
        print(f"Build directory for cached build does not exists {build_dir}")
        os.mkdir(build_dir)
        print(f"Cached build will be REBUILT")
        build_info.mark_cached(False)

    os.chdir(build_dir)

    #Run CMake Commands
    run_cmake(project_dir, build_info.get_cmake_flags())
    run_make(build_info.get_env_vars())

    os.chdir(project_dir)

def run_cmake(cmake_lists_dir: str, cmake_build_commands: List[str]) -> None:
    #CMake source and build directory parameters
    #https://stackoverflow.com/questions/18826789/cmake-output-build-directory

    print("~~~~~~~~~~CMAKE Config~~~~~~~~~~")
    subprocess_check_call(["cmake"] + [f"-S{cmake_lists_dir}"] + cmake_build_commands) 
    print("~~~~~~~~~~CMAKE Config has completed successfully~~~~~~~~~~")

def run_git_info() -> None:
    #We call run here because check_call should not capture stdout
    #https://docs.python.org/3/library/subprocess.html#subprocess.run
    ret = subprocess_run(["git", "rev-parse", "--abbrev-ref", "HEAD"], subprocess.PIPE)
    print(f"Running off branch: {ret.stdout.decode('UTF-8').rstrip()}")

def run_make(env_args: List[str]):
    #https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    print("~~~~~~~~~~Make~~~~~~~~~~")
    subprocess_check_call(["make","-j", "{}".format(max(1, multiprocessing.cpu_count() -2))])
    print("~~~~~~~~~~Make has completed successfully~~~~~~~~~~")

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
        args = check_default_args(args_dict)

        build_dir: str = args[BUILD_DIR_FLAG_KEY]

        cmake_build_commands = Parser.parse_cmake_args(args)
        env_args = Parser.parse_build_env(args)

        build_info = BuildInfo(build_dir, cmake_build_commands, env_args)
    elif len(args_dict) == 0:
        # Case 2
        print("Using previously cached build")
    elif len(args_dict) == 1 and CLEAN_FLAG_KEY in args_dict:
        # Case 3
        print("Using cached build and cleaning")
    else:
        # Case 4 What defines new
        pass


    #TODO Handle the case where the build is different then the currently active build

    return build_info

def subprocess_check_call(cmd: List[str]):
    try:
        subprocess.check_call(cmd)
    except Exception as e:
        logging.error("\n\n")
        logging.error(e)
        logging.error("\n\n")
        logging.error("<-----------------------------------Exception occurred when running----------------------------------->")
        logging.error(" ".join(cmd))

        exit(EXIT_CODE_FAIL)


def subprocess_run(cmd: List[str], stdout = None) -> subprocess.CompletedProcess:
    try:
        ret = subprocess.run(cmd, check = True, stdout=stdout)
        return ret
    except Exception as e:
        logging.error("\n\n")
        logging.error(e)
        logging.error("\n\n")
        logging.error("<-----------------------------------Exception occurred when running----------------------------------->")
        logging.error(" ".join(cmd))

        exit(EXIT_CODE_FAIL)

if __name__ == "__main__":
    perform_build(parse_args())
