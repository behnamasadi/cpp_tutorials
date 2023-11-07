#!/bin/sh

find sources src includes tests examples \( -name '*.cpp' -o -name '*.hpp' -o -name '*.ipp' -o -name '*.c' -o -name '*.h' \) -exec sh -c 'echo "Formatting file: $1  with LLVM style "; clang-format --style=llvm -i "$1"' sh {} ';'

