#!/bin/bash

dir=$1

[ -z "$dir" ] && echo "directory is empty" && exit 1
[ ! -d "$dir" ] && echo "$dir is invalid" && exit 1

echo "formatting directory: $dir" 

find $dir -type f \( -name \*.c* -o -name \*.h* \) \
            | xargs clang-format -i \
            -style="{BasedOnStyle: Google, DerivePointerAlignment: false, PointerAlignment: Left}"
