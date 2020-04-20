#!/usr/bin/env python3

import os
import argparse
import subprocess

EXCLUSION_LIST = ["third_party", ".git"]
INCLUSION_LIST = (".cxx", ".hxx")

APPLY_KEY = "apply"

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", help="Apply the formatting to the files", action="store_true")
    args = parser.parse_args()

    ret = dict()

    if args.apply:
        print("Formatting will NOT be applied to the source files")
        ret[APPLY_KEY] = "-i"
    else:
        print("Formatting will NOT be applied to the source files")
        ret[APPLY_KEY] = ""

    return ret


def format(args_dict):
    cur_dir = os.getcwd()

    print("Excluding files in following folders: {}", EXCLUSION_LIST)
    format_files = list()

    for (dir_path, sub_dirs, filenames) in os.walk(cur_dir):
        sub_dirs[:] = list(filter(lambda d: d not in EXCLUSION_LIST, sub_dirs))
        valid_files = list(filter(lambda f: f.endswith(INCLUSION_LIST), filenames))

        format_files += list(map(lambda f: os.path.join(dir_path, f), valid_files))

    for format_file in format_files:
        print("Formatting {}".format(format_file))
        subprocess.run("clang-format -style=file {} {}".format(format_file, args_dict[APPLY_KEY]), shell=True, check=True)

if __name__ == "__main__":
    format(parse_args())
