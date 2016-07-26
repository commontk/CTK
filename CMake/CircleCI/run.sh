#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/usr/src/CTK \
    commontk/ctk-test \
/usr/src/CTK/CMake/CircleCI/test.sh