#!/usr/bin/env python3

import os
import subprocess

EXCLUSION_LIST = ["third_party", ".git"]
INCLUSION_LIST = (".cxx", ".hxx")

def format():
    cur_dir = os.getcwd()

    print("Excluding files in following folders: {}", EXCLUSION_LIST)
    format_files = list()


    for (dir_path, sub_dirs, filenames) in os.walk(cur_dir):
        sub_dirs[:] = list(filter(lambda d: d not in EXCLUSION_LIST, sub_dirs))
        valid_files = list(filter(lambda f: f.endswith(INCLUSION_LIST), filenames))

        format_files += list(map(lambda f: os.path.join(dir_path, f), valid_files))

    for format_file in format_files:
        print("Formatting {}".format(format_file))
        subprocess.run("clang-format -style=file {} -i".format(format_file), shell=True, check=True)

if __name__ == "__main__":
    format()
