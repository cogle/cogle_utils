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
from typing import Dict, List, Optional

logging.basicConfig(level=logging.ERROR)

TESTS_FLAG_KEY = "tests"
TSAN_FLAG_KEY = "tsan"
ASAN_FLAG_KEY = "asan"
CLEAN_FLAG_KEY = "clean"
COMPILER_FLAG_KEY = "compiler"
WIPE_FLAG_KEY = "wipe"
GCOV_FLAG_KEY = "gcov"

# Key Pairs
# TODO Try to coalesce into a single flag
BUILD_FLAG_KEY = "build"
CMAKE_BUILD_FLAG_KEY = "cmake_build_flag"

BUILD_DIR_FLAG_KEY = "cmake_build_dir"
BUILD_DIR_CMAKE_FLAG_KEY = "cmake_build_dir_flag"

# TODO PUT EACH CMD in array for debugging purposes(subprocess)
# TODO SUPPORT CORES FLAG
# TODO SUPPORT ADDITION CMAKE ARGS PASSED IN BY USER
# TODO SUPPORT MULTIPLE BUILDS(JSON)
# TODO MAKE SOURCE DIR CONFIGURABLE

CMAKE_BUILD_ARGS_KEYS_SET = {CMAKE_BUILD_FLAG_KEY, TESTS_FLAG_KEY,
                             TSAN_FLAG_KEY, ASAN_FLAG_KEY, BUILD_DIR_CMAKE_FLAG_KEY, GCOV_FLAG_KEY}
BUILD_ENV_KEYS_SET = {COMPILER_FLAG_KEY}

REQUIRED_KEYS = {BUILD_FLAG_KEY, BUILD_DIR_FLAG_KEY, COMPILER_FLAG_KEY}

# Add your CMake Flags here
TSAN_BUILD = "-DWITH_TSAN=true"
ASAN_BUILD = "-DWITH_ASAN=true"
GCOV_BUILD = "-DWITH_GCOV=true"

UNIT_TESTS_BUILD = "-DWITH_TESTS=true"

EXIT_CODE_FAIL = -1

DEFAULT_LINE_WIDTH = 100

COVERAGE_EXCLUDES_LIST = ["*third_party/*"]


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


class BuildType(enum.Enum):
    DEBUG = 0
    RELEASE = 1


class CompilerType(enum.Enum):
    GNU = 0
    CLANG = 1


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
            ret.append(str(v))

        return ret

    def get_env_vars(self) -> Dict[str, str]:
        env = os.environ.copy()
        for k, v in self.env_vars.items():
            if k == COMPILER_FLAG_KEY:
                if v == CompilerType.GNU:
                    which_gxx = subprocess_run(
                        ["which", "g++"], subprocess.PIPE)
                    gxx = which_gxx.stdout.decode('UTF-8').rstrip()

                    which_g = subprocess_run(["which", "gcc"], subprocess.PIPE)
                    g = which_g.stdout.decode('UTF-8').rstrip()

                    if len(gxx) == 0 or len(g) == 0:
                        logging.critical("GNU compiler not found")
                        exit(EXIT_CODE_FAIL)

                    env["CC"] = g
                    env["CXX"] = gxx
                elif v == CompilerType.CLANG:
                    which_clangxx = subprocess_run(
                        ["which", "clang++"], subprocess.PIPE)
                    clangxx = which_clangxx.stdout.decode('UTF-8').rstrip()

                    which_clang = subprocess_run(
                        ["which", "clang"], subprocess.PIPE)
                    clang = which_clang.stdout.decode('UTF-8').rstrip()

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


def check_default_args(args_dict):
    """Ensure that the passed in args from the user have the proper
    default arguments set.
    """

    validated_args_dict = args_dict

    for k in REQUIRED_KEYS:
        if k not in args_dict:
            if k == BUILD_FLAG_KEY:
                validated_args_dict[k] = BuildType.DEBUG
                validated_args_dict[
                    CMAKE_BUILD_FLAG_KEY] = f"-DCMAKE_BUILD_TYPE={BuildType.DEBUG.name}"
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


def format_build_str(print_str: str, line_len: int = DEFAULT_LINE_WIDTH, fill_char: str = "#") -> None:
    print(print_str.center(line_len, fill_char))


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--dir", help="Directory to place build folder in", action="store", type=str)
    parser.add_argument(
        "--clang", help="Use clang++ compiler to build(default)", action="store_true")
    parser.add_argument(
        "--gnu", help="Use g++ compiler to build", action="store_true")
    parser.add_argument(
        "--tsan", help="Build using tsan utility to check thread safety", action="store_true")
    parser.add_argument(
        "--asan", help="Build using asan utility to check memory safety", action="store_true")
    parser.add_argument(
        "--gcov", help="Build using gcov utility to check code coverage", action="store_true")
    parser.add_argument(
        "--debug", help="Build using debug version(default)", action="store_true")
    parser.add_argument(
        "--release", help="Build using release version", action="store_true")
    parser.add_argument(
        "--tests", help="Build with unit tests", action="store_true")
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

    # Check and ensure that either GNU or Clang is selected
    if args.gnu and args.clang:
        parser.error(
            "Unable to build with both G++ and Clang++ please specify only ONE")

    # Check and ensure that either Debug or Release is selected.
    if args.debug and args.release:
        parser.error(
            "Unable to build with both G++ and Clang++ please specify only ONE")

    # Check and ensure that either ASAN or TSAN is selected.
    if args.asan and args.tsan:
        parser.error(
            "Unable to build with both ASAN and TSAN please specify only ONE")

    # Assign all variables based upon input parameters

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

    # wipe
    if args.wipe:
        ret[WIPE_FLAG_KEY] = True
    else:
        ret[WIPE_FLAG_KEY] = False

    # tsan
    if args.tsan:
        ret[TSAN_FLAG_KEY] = TSAN_BUILD

    # asan
    if args.asan:
        ret[ASAN_FLAG_KEY] = ASAN_BUILD

    # asan
    if args.gcov:
        ret[GCOV_FLAG_KEY] = GCOV_BUILD

    # unit tests
    if args.tests:
        ret[TESTS_FLAG_KEY] = UNIT_TESTS_BUILD

    # build dir
    if args.dir:
        if not os.path.exists(args.dir):
            raise parser.error(
                f"Directory {args.dir} does not exists please create and run again.")

        ret[BUILD_DIR_CMAKE_FLAG_KEY] = f"-B{args.dir}"
        ret[BUILD_DIR_FLAG_KEY] = f"{args.dir}"

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
    format_build_str("CMAKE Config", fill_char="~")
    subprocess_check_call(
        ["cmake"] + [f"-S{cmake_lists_dir}"] + cmake_build_commands, env=env_args)
    format_build_str("CMAKE Config has completed successfully", fill_char="~")
    print_new_line()


def run_genhtml(coverage_file_url: str, report_out_dir: str) -> None:
    format_build_str("Running genhtml", fill_char="~")
    subprocess_check_call(
        ["genhtml", coverage_file_url,  "--output-directory", report_out_dir])
    format_build_str("LCOV has completed successfully", fill_char="~")


def run_lcov(project_directory: str, build_dir: str, coverage_file_url: str, excludes_dirs: List[str] = COVERAGE_EXCLUDES_LIST) -> None:
    format_build_str("Running LCOV", fill_char="~")
    subprocess_check_call(["lcov", "--capture", "--directory", build_dir, "--output-file",
                           coverage_file_url, "--no-external", "--base-directory", project_directory, "--exclude"] + excludes_dirs)
    format_build_str("LCOV has completed successfully", fill_char="~")


def run_git_info() -> None:
    # We call run here because check_call should not capture stdout
    # https://docs.python.org/3/library/subprocess.html#subprocess.run
    ret = subprocess_run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"], subprocess.PIPE)
    print(f"Running off branch: {ret.stdout.decode('UTF-8').rstrip()}")


def run_make(env_dict: Dict[str, str]):
    # https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    format_build_str("Make", fill_char="~")
    subprocess_check_call(
        ["make", "-j", "{}".format(max(1, multiprocessing.cpu_count() - 2))], env=env_dict)
    format_build_str("Make has completed successfully", fill_char="~")
    print_new_line()


def run_make_clean(env_dict: Dict[str, str]):
    format_build_str("Make Clean", fill_char="~")
    subprocess_check_call(["make", "clean"], env=env_dict)
    format_build_str("Make Clean has completed successfully", fill_char="~")
    print_new_line()


def run_tests():
    format_build_str("Running Unit Tests", fill_char="-")
    subprocess_check_call(["ctest", "--verbose"])


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
