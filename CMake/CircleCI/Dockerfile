FROM thewtex/opengl:v0.1.0
MAINTAINER Insight Software Consortium <community@itk.org>

RUN apt-get update && apt-get install -y \
  qt4-dev-tools \
  cmake

ENV APP "sudo chown -R user.user /usr/src/ && /usr/src/CTK/CMake/CircleCI/test.sh"
