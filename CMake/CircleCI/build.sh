#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker build -t commontk/ctk-test:opengl $script_dir
