#!/bin/bash

# This is a script to build the modules and run the test suite in the base
# Docker container.

die() {
  echo "Error: $@" 1>&2
  exit 1;
}

mkdir /usr/src/CTK-build
cd /usr/src/CTK-build || die "Could not cd into the build directory"

mkdir /usr/src/CTK-build/CTK-build
cd /usr/src/CTK-build/CTK-build || die "Could not cd into the build directory"

ctest \
  $1 \
  -S /usr/src/CTK/CMake/CircleCI/CircleCI_CTK_Docker.cmake \
  -VV || die "ctest failed"
