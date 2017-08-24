#!/bin/bash

set -e
set -o pipefail

script_dir="`cd $(dirname $0); pwd`"

$script_dir/run_opengl.sh -i commontk/ctk-test:opengl-$1 -c opengl-$1 -p 6081 -r --env="CIRCLE_SHA1=$2" -r --env="CIRCLE_BRANCH=$3" -r --env="SITE_BUILD_TYPE=$4"
