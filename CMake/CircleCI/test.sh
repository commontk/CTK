#!/bin/bash

# This is a script to build the modules and run the test suite in the base
# Docker container.

die() {
  echo "Error: $@" 1>&2
  exit 1;
}

mkdir /usr/src/CTK-build
cd /usr/src/CTK-build || die "Could not cd into the build directory"

cmake \
  -DCMAKE_BUILD_TYPE:STRING=Release \
  -DBUILD_TESTING:BOOL=ON \
    /usr/src/CTK || die "CMake configuration failed"

make -j3 || die "examples build failed"

cd CTK-build
ctest -VV -D Experimental || die "ctest failed"
