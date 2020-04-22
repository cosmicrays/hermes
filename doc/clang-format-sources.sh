#!/bin/bash
projectrootdir="$(dirname pwd)"
cd $projectrootdir
git ls-files -- '*.cpp' '*.h' | grep "^\(include\|src\|python\|test\)" |  xargs clang-format -i --style="{BasedOnStyle: llvm, IndentWidth: 8, UseTab: Always}"
