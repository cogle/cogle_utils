#!/usr/bin/env python3

import argparse
import subprocess

COMPILER_FLAG_KEY = "compiler"
TEST_FLAG_KEY = "test"

GNU_COMPILER_FLAG_SELECT_VAL   = "gnu"
CLANG_COMPILER_FLAG_SELECT_VAL = "clang"

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang", help="Use clang++ compiler to build(default)", action="store_true")
    parser.add_argument("--gnu", help="Use g++ compiler to build", action="store_true")
    parser.add_argument("--tsan", help="Build using tsan utility to check thread safety", action="store_true")
    parser.add_argument("--asan", help="Build using asan utility to check memory safety", action="store_true")
    args = parser.parse_args()

    ret = dict()

    #Check all parser arg errors/validity below
    if args.gnu and args.clang:
        parser.error("Unable to build with both G++ and Clang++ please specify only ONE")

    #Assign all variables based upon input parameters
    if args.gnu:
        ret[COMPILER_FLAG_KEY] = GNU_COMPILER_FLAG_SELECT_VAL
    else:
        ret[COMPILER_FLAG_KEY] = CLANG_COMPILER_FLAG_SELECT_VAL

    return ret

def setup_build_env():
    #https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
    pass

def perform_build(args_dict):
    build_env = setup_build_env()
    pass

if __name__ == "__main__":
    perform_build(parse_args())