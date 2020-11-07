#!/usr/bin/env python3

import os
import sys
import enum
import json
import argparse
import subprocess
import logging
import shutil
import multiprocessing

from pathlib import Path
from typing import Dict, List, Optional, Tuple

logging.basicConfig(level=logging.ERROR)

TESTS_FLAG_KEY = "tests"
SANITIZER_FLAG_KEY = "sanitizer"
CLEAN_FLAG_KEY = "clean"
COMPILER_FLAG_KEY = "compiler"
WIPE_FLAG_KEY = "wipe"
GCOV_FLAG_KEY = "gcov"
EXAMPLES_FLAG_KEY = "examples"
CMAKE_USER_DEFINITIONS = "user_args"

# Key Pairs
# TODO Try to coalesce into a single flag
BUILD_FLAG_KEY = "build"
CMAKE_BUILD_FLAG_KEY = "cmake_build_flag"

BUILD_DIR_FLAG_KEY = "cmake_build_dir"
BUILD_DIR_CMAKE_FLAG_KEY = "cmake_build_dir_flag"

# TODO PUT EACH CMD in array for debugging purposes(subprocess)
# TODO SUPPORT CORES FLAG
# TODO SUPPORT MULTIPLE BUILDS(JSON)
# TODO MAKE SOURCE DIR CONFIGURABLE

CMAKE_BUILD_ARGS_KEYS_SET = {CMAKE_BUILD_FLAG_KEY, TESTS_FLAG_KEY, CMAKE_USER_DEFINITIONS,
                             SANITIZER_FLAG_KEY, BUILD_DIR_CMAKE_FLAG_KEY, GCOV_FLAG_KEY, EXAMPLES_FLAG_KEY}
BUILD_ENV_KEYS_SET = {COMPILER_FLAG_KEY}

REQUIRED_KEYS = {BUILD_FLAG_KEY, BUILD_DIR_FLAG_KEY, COMPILER_FLAG_KEY}

# Add your CMake Flags here
GCOV_BUILD = "-DWITH_GCOV=true"

UNIT_TESTS_BUILD = "-DWITH_TESTS=true"
EXAMPLES_BUILD = "-DWITH_EXAMPLES=true"

EXIT_CODE_FAIL = -1

DEFAULT_LINE_WIDTH = 100

COVERAGE_EXCLUDES_LIST = ["*third_party/*", "*/tests/*"]
CMAKE_CACHE_FILE = "CMakeCache.txt"
CMAKE_CACHE_FILE_COMPILER_STRING = "CMAKE_CXX_COMPILER:FILEPATH="


class FlagsExtractor:
    @staticmethod
    def extract_cmake_args(args_dict):
        cmake_args = dict()

        for k in args_dict:
            if k in CMAKE_BUILD_ARGS_KEYS_SET:
                cmake_args[k] = args_dict[k]

        return cmake_args

    # TODO: The above returns an Dict[str, str] symmetric calls would be nice
    @staticmethod
    def extract_build_env(args_dict):
        build_env = dict()

        for k in args_dict:
            if k in BUILD_ENV_KEYS_SET:
                build_env[k] = args_dict[k]

        return build_env


class ExtendedEnum(enum.Enum):
    @classmethod
    def list(cls):
        return list(map(lambda c: c.value.lower(), cls))


class Sanitizers(ExtendedEnum):
    ASAN = "ASAN"
    TSAN = "TSAN"
    UBSAN = "UBSAN"

    def create_cmake_sanitizer_str(self):
        if self.ASAN:
            return "-DWITH_ASAN=true"
        elif self.TSAN:
            return "-DWITH_TSAN=true"
        elif self.UBSAN:
            return "-DWITH_UBSAN=true"
        else:
            return ""


class BuildTypes(ExtendedEnum):
    DEBUG = "DEBUG"
    RELEASE = "RELEASE"

    def create_cmake_build_type_str(self):
        return f"-DCMAKE_BUILD_TYPE={self.value}"


class Compilers(ExtendedEnum):
    GNU = "GNU"
    CLANG = "CLANG"

    @staticmethod
    def gnu_compiler_info() -> Tuple[str, str]:
        return tuple(Compilers.which_gxx(), Compilers.which_gcc())

    @staticmethod
    def clang_compiler_info() -> Tuple[str, str]:
        return tuple(Compilers.which_clangxx(), Compilers.which_clang())

    @staticmethod
    def which_gxx() -> str:
        which_gxx = subprocess_run(
            ["which", "g++"], subprocess.PIPE)
        gxx = which_gxx.stdout.decode('UTF-8').rstrip()

        return gxx

    @staticmethod
    def which_gcc() -> str:
        which_gcc = subprocess_run(["which", "gcc"], subprocess.PIPE)
        gcc = which_gcc.stdout.decode('UTF-8').rstrip()

        return gcc

    @staticmethod
    def which_clangxx() -> str:
        which_clangxx = subprocess_run(
            ["which", "clang++"], subprocess.PIPE)
        clangxx = which_clangxx.stdout.decode('UTF-8').rstrip()

        return clangxx

    @staticmethod
    def which_clang() -> str:
        which_clang = subprocess_run(
            ["which", "clang"], subprocess.PIPE)
        clang = which_clang.stdout.decode('UTF-8').rstrip()

        return clang


class BuildInfo:
    def __init__(self, build_dir: str, do_clean: bool, cmake_flags: Dict[str, str], env_vars: Dict[str, str]):
        self.cmake_flags = cmake_flags
        self.env_vars = env_vars
        self.build_dir = build_dir

        self.is_cached = False
        self.clean = do_clean

    def get_build_dir(self) -> str:
        return self.build_dir

    def get_clean(self) -> bool:
        return self.clean

    def get_cached(self) -> bool:
        return self.is_cached

    def get_cmake_flags(self) -> List[str]:
        ret = list()
        for _, v in self.cmake_flags.items():
            ret += str(v).strip().split(" ")
        return ret

    def get_env_vars(self) -> Dict[str, str]:
        env = os.environ.copy()
        for k, v in self.env_vars.items():
            if k == COMPILER_FLAG_KEY:
                if v == Compilers.GNU:
                    gxx = Compilers.which_gxx()
                    gcc = Compilers.which_gcc()

                    if len(gxx) == 0 or len(gcc) == 0:
                        logging.critical("GNU compiler not found")
                        exit(EXIT_CODE_FAIL)

                    env["CC"] = gcc
                    env["CXX"] = gxx
                elif v == Compilers.CLANG:
                    clangxx = Compilers.which_clangxx()
                    clang = Compilers.which_clang()

                    if len(clangxx) == 0 or len(clang) == 0:
                        logging.critical("Clang compiler not found")
                        exit(EXIT_CODE_FAIL)

                    env["CC"] = clang
                    env["CXX"] = clangxx
                else:
                    logging.critical(f"Unknown compiler type {v}")
                    exit(EXIT_CODE_FAIL)

        return env

    def mark_cached(self, is_cached: bool) -> None:
        self.is_cached = is_cached

    def run_code_coverage(self) -> bool:
        return GCOV_FLAG_KEY in self.cmake_flags

    def run_tests(self) -> bool:
        return TESTS_FLAG_KEY in self.cmake_flags


def check_compiler(build_dir: str, compiler_type: Compilers) -> bool:
    """Checks if the incoming compiler version is the same as the 
    previously used one in the CMakeCache.txt, compiler switching
    requires an environment variable to be set.
    Returns: True if ok, False if clean is needed.
    """
    cmake_cache_file_path = Path(build_dir)/CMAKE_CACHE_FILE
    if not os.path.isfile(cmake_cache_file_path):
        return True

    search_line = ""
    with open(cmake_cache_file_path, 'r') as f:
        for line in f:
            if CMAKE_CACHE_FILE_COMPILER_STRING in line:
                search_line = line

    if compiler_type == Compilers.GNU:
        path = Compilers.which_gxx()

        if path not in search_line:
            return False

    if compiler_type == Compilers.CLANG:
        path = Compilers.which_clangxx()

        if path not in search_line:
            return False

    return True


def check_default_args(args_dict):
    """Ensure that the passed in args from the user have the proper
    default arguments set.
    """

    validated_args_dict = args_dict

    for k in REQUIRED_KEYS:
        if k not in args_dict:
            if k == BUILD_FLAG_KEY:
                debug_build_type_enum = BuildTypes.DEBUG
                validated_args_dict[k] = debug_build_type_enum
                validated_args_dict[
                    CMAKE_BUILD_FLAG_KEY] = debug_build_type_enum.create_cmake_build_type_str()
            elif k == BUILD_DIR_FLAG_KEY:
                build_dir = os.path.join(os.getcwd(), "build")

                validated_args_dict[k] = build_dir
                validated_args_dict[BUILD_DIR_CMAKE_FLAG_KEY] = f"-B{build_dir}"
            elif k == COMPILER_FLAG_KEY:
                validated_args_dict[k] = Compilers.CLANG
            else:
                print(f"Key {k} is required but is not handled")
                exit(EXIT_CODE_FAIL)

    return validated_args_dict


def print_centered(print_str: str, line_len: int = DEFAULT_LINE_WIDTH, fill_char: str = "#") -> None:
    print(print_str.center(line_len, fill_char))


def generate_lcov_excludes(excludes_list: List[str]) -> List[str]:
    ret = list()

    for exclusion in excludes_list:
        ret.append("--exclude")
        ret.append(exclusion)

    return ret


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--custom_defs", help="User specified CMake Definitions", action="store", type=str)
    parser.add_argument(
        "--dir", help="Directory to place build folder in", action="store", type=str)
    parser.add_argument(
        "--compiler", help="Supported compilers: GNU,Clang", type=str.lower, choices=Compilers.list())
    parser.add_argument(
        "--sanitizer", help="Supported sanitizers: ASAN,TSAN,UBSAN", type=str.lower, choices=Sanitizers.list())
    parser.add_argument(
        "--build_type", help="Supported build types: DEBUG, RELEASE", type=str.lower, choices=BuildTypes.list())
    parser.add_argument(
        "--gcov", help="Build using gcov utility to check code coverage", action="store_true")
    parser.add_argument(
        "--tests", help="Build with unit tests", action="store_true")
    parser.add_argument(
        "--examples", help="Build with examples", action="store_true")
    parser.add_argument("--clean", help="Build clean", action="store_true")
    parser.add_argument(
        "--wipe", help="Wipes the build directory by removing it", action="store_true")

    args = parser.parse_args()

    ret = dict()

    # Extract all parser args below

    # If there is nothing then early terminate and attempt to use the cached version
    if sys.argv == 1:
        return ret

    # make clean
    if args.clean:
        ret[CLEAN_FLAG_KEY] = True
    else:
        ret[CLEAN_FLAG_KEY] = False

    # Assign all variables based upon input parameters

    # Compiler Selection
    if args.compiler:
        ret[COMPILER_FLAG_KEY] = Compilers(args.compiler.upper())

    # Build Type Selection
    if args.build_type:
        build_type_enum = BuildTypes(args.build_types.upper())
        ret[BUILD_FLAG_KEY] = build_type_enum
        ret[CMAKE_BUILD_FLAG_KEY] = build_type_enum.create_cmake_build_type_str()

    # wipe
    if args.wipe:
        ret[WIPE_FLAG_KEY] = True
    else:
        ret[WIPE_FLAG_KEY] = False

    # sanitizers
    if args.sanitizer:
        sanitizer_enum = args.sanitizer.upper()
        #ret[SANITIZER_FLAG_KEY] = Sanitizers()

    # gcov
    if args.gcov:
        ret[GCOV_FLAG_KEY] = GCOV_BUILD

    # unit tests
    if args.tests:
        ret[TESTS_FLAG_KEY] = UNIT_TESTS_BUILD

    # examples
    if args.examples:
        ret[EXAMPLES_FLAG_KEY] = EXAMPLES_BUILD

    # build dir
    if args.dir:
        if not os.path.exists(args.dir):
            raise parser.error(
                f"Directory {args.dir} does not exists please create and run again.")

        ret[BUILD_DIR_CMAKE_FLAG_KEY] = f"-B{args.dir}"
        ret[BUILD_DIR_FLAG_KEY] = f"{args.dir}"

    # user specified CMake Definitions
    if args.custom_defs:
        ret[CMAKE_USER_DEFINITIONS] = args.custom_defs

    return ret


def perform_build(args_dict) -> None:
    project_dir = os.getcwd()

    run_git_info()

    build_info = setup_build_args(args_dict, project_dir)
    logging.debug(f"{build_info.__dict__}")

    build_dir = build_info.get_build_dir()

    if args_dict[WIPE_FLAG_KEY] and os.path.exists(build_dir):
        print(f"Clearing the build directory {build_dir}")
        shutil.rmtree(build_dir)

    compiler_is_same = True

    if COMPILER_FLAG_KEY in args_dict and os.path.exists(build_dir):
        compiler_is_same = check_compiler(
            build_dir, args_dict[COMPILER_FLAG_KEY])

    if not compiler_is_same and os.path.exists(build_dir):
        print(
            f"Clearing the build directory {build_dir} due to compiler change")
        shutil.rmtree(build_dir)

    if not build_info.get_cached() and not os.path.exists(build_dir):
        print(f"Creating {build_dir}")
        os.mkdir(build_dir)

    if build_info.get_cached() and not os.path.exists(build_dir):
        print(f"Build directory for cached build does not exists {build_dir}")
        os.mkdir(build_dir)
        print(f"Cached build will be REBUILT")
        build_info.mark_cached(False)

    os.chdir(build_dir)

    build_env = build_info.get_env_vars()

    # Run CMake Commands
    run_cmake(project_dir, build_info.get_cmake_flags(), build_env)

    if build_info.get_clean():
        run_make_clean(build_env)

    run_make(build_env)

    if build_info.run_tests():
        run_tests()

    if build_info.run_code_coverage():
        # Clean previous coverage report
        coverage_report_dir = Path(build_dir) / "coverage"
        if os.path.exists(coverage_report_dir):
            print("Removing previous coverage report directory")
            shutil.rmtree(coverage_report_dir)

        os.mkdir(coverage_report_dir)

        coverage_file_url = coverage_report_dir / "coverage.info"
        report_out_dir = coverage_report_dir / "out"

        run_lcov(project_dir, str(build_dir), str(coverage_file_url))
        run_genhtml(str(coverage_file_url), str(report_out_dir))

    os.chdir(project_dir)


def print_new_line(num_lines: int = 1):
    print("\n"*num_lines)


def run_cmake(cmake_lists_dir: str, cmake_build_commands: List[str], env_args: Dict[str, str]) -> None:
    # CMake source and build directory parameters
    # https://stackoverflow.com/questions/18826789/cmake-output-build-directory
    logging.debug(f"CMake {cmake_build_commands}")
    print_centered("CMAKE Config", fill_char="~")
    subprocess_check_call(
        ["cmake"] + [f"-S{cmake_lists_dir}"] + cmake_build_commands, env=env_args)
    print_centered("CMAKE Config has completed successfully", fill_char="~")
    print_new_line()


def run_genhtml(coverage_file_url: str, report_out_dir: str) -> None:
    print_centered("Running genhtml", fill_char="~")
    subprocess_check_call(
        ["genhtml", coverage_file_url,  "--output-directory", report_out_dir])
    print_centered("LCOV has completed successfully", fill_char="~")


def run_lcov(project_directory: str, build_dir: str, coverage_file_url: str, excludes_dirs: List[str] = COVERAGE_EXCLUDES_LIST) -> None:
    print_centered("Running LCOV", fill_char="~")
    subprocess_check_call(["lcov", "--capture", "--directory", build_dir, "--output-file",
                           coverage_file_url, "--no-external", "--base-directory", project_directory] + generate_lcov_excludes(excludes_dirs))
    print_centered("LCOV has completed successfully", fill_char="~")


def run_git_info() -> None:
    # We call run here because check_call should not capture stdout
    # https://docs.python.org/3/library/subprocess.html#subprocess.run
    ret = subprocess_run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"], subprocess.PIPE)
    print(f"Running off branch: {ret.stdout.decode('UTF-8').rstrip()}")


def run_make(env_dict: Dict[str, str]):
    # https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    print_centered("Make", fill_char="~")
    subprocess_check_call(
        ["make", "-j", "{}".format(max(1, multiprocessing.cpu_count() - 2))], env=env_dict)
    print_centered("Make has completed successfully", fill_char="~")
    print_new_line()


def run_make_clean(env_dict: Dict[str, str]):
    print_centered("Make Clean", fill_char="~")
    subprocess_check_call(["make", "clean"], env=env_dict)
    print_centered("Make Clean has completed successfully", fill_char="~")
    print_new_line()


def run_tests():
    print_centered("Running Unit Tests", fill_char="-")
    subprocess_check_call(["ctest", "--verbose", "--stop-on-failure"])


def setup_build_args(args_dict, project_dir: str) -> BuildInfo:
    # Cases
    # 1) Parse Incoming args and setup build based upon that.
    # 2) TODO Handle clean and some sort of caching

    build_info: BuildInfo = None
    args = check_default_args(args_dict)

    build_dir: str = args[BUILD_DIR_FLAG_KEY]

    cmake_build_commands = FlagsExtractor.extract_cmake_args(args)
    env_args = FlagsExtractor.extract_build_env(args)

    build_info = BuildInfo(
        build_dir, args_dict[CLEAN_FLAG_KEY], cmake_build_commands, env_args)

    return build_info


def subprocess_check_call(cmd: List[str], env=None):
    try:
        subprocess.check_call(cmd, env=env)
    except Exception as e:
        logging.error("\n\n")
        logging.critical(e)
        logging.error("\n\n")
        logging.critical(
            "<-----------------------------------Exception occurred when running----------------------------------->")
        logging.critical(" ".join(cmd))

        exit(EXIT_CODE_FAIL)


def subprocess_run(cmd: List[str], stdout=None) -> subprocess.CompletedProcess:
    try:
        ret = subprocess.run(cmd, check=True, stdout=stdout)
        return ret
    except Exception as e:
        logging.error("\n\n")
        logging.critical(e)
        logging.error("\n\n")
        logging.critical(
            "<-----------------------------------Exception occurred when running----------------------------------->")
        logging.critical(" ".join(cmd))

        exit(EXIT_CODE_FAIL)


if __name__ == "__main__":
    perform_build(parse_args())
