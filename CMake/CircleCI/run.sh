#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

$script_dir/run_opengl.sh -i commontk/ctk-test:opengl -p 6081 "$@"
