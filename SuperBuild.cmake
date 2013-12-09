###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################


#-----------------------------------------------------------------------------
# WARNING - No change should be required after this comment
#           when you are adding a new external project dependency.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Declare CTK_EXTERNAL_LIBRARY_DIRS variable - This variable stores
# the library output directory associated with the different external project
# It's then used in CMake/LastConfigureStep/CTKGenerateCTKConfig.cmake to
# configure CTKConfig.cmake.in
# This variable would then be exposed to project building against CTK
set(CTK_EXTERNAL_LIBRARY_DIRS)

#-----------------------------------------------------------------------------
# Make sure ${CTK_BINARY_DIR}/CTK-build/bin exists
# May be used by some external project to install libs 
if(NOT EXISTS ${CTK_BINARY_DIR}/CTK-build/bin)
  file(MAKE_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
endif()

#-----------------------------------------------------------------------------
set(proj CTK)

ExternalProject_Add(${proj}
  ${CTK_EXTERNAL_PROJECT_ARGS}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  LIST_SEPARATOR ${sep}
  CMAKE_CACHE_ARGS
    -DCTK_SUPERBUILD:BOOL=OFF
    -DCTK_SUPERBUILD_BINARY_DIR:PATH=${CTK_BINARY_DIR}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS_INIT:STRING=${CMAKE_CXX_FLAGS_INIT}
    -DCMAKE_C_FLAGS_INIT:STRING=${CMAKE_C_FLAGS_INIT}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCTK_EXTERNAL_LIBRARY_DIRS:STRING=${CTK_EXTERNAL_LIBRARY_DIRS}
  SOURCE_DIR ${CTK_SOURCE_DIR}
  BINARY_DIR ${CTK_BINARY_DIR}/CTK-build
  INSTALL_COMMAND ""
  DEPENDS
    ${CTK_DEPENDENCIES}
  STEP_TARGETS configure
  )
