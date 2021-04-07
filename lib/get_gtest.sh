#!/bin/sh

git clone --branch release-1.10.0 --depth 1 https://github.com/google/googletest.git gtest
cd gtest
find . -maxdepth 1 ! -name 'googletest' -type f -exec rm -f {} +
rm -rf googlemock ci .git docs
