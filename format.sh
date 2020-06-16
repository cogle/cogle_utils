#!/bin/bash


format_find_command() {
    local list=$1 
    local path=$2

    local ret=""
    for i in $(echo ${list} | tr "," "\n")
    do
        ret+="-path ${path}/${i} -prune -o "
    done

    echo ${ret} | xargs

    return 0
}


cur_dir="."

# Add any directory you would like excluded from 
exclude_dirs_list="third_party,.git"
formatted_exclude_dirs=$( format_find_command ${exclude_dirs_list} ${cur_dir} )

files_to_format=`find ${cur_dir} ${formatted_exclude_dirs} -type f -print`

for file in $files_to_format
do
    if [[ "${file}" =~ ".hxx" || "${file}" =~ ".cxx" ]] ; then
        echo "Formatting ${file}"
    fi
done