#!/bin/sh

git clone https://github.com/google/googletest.git gtest
cd gtest
find . -maxdepth 1 ! -name 'googletest' -type f -exec rm -f {} + 
