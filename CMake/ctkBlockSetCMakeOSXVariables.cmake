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
#      http://www.commontk.org/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# ctkBlockSetCMakeOSXVariables
#

#
# Adapted from Paraview/Superbuild/CMakeLists.txt
#

# Note: Change architecture *before* any enable_language() or project()
#       calls so that it's set properly to detect 64-bit-ness...
#       Currently, we default to 10.5. Obviously this may need to be re-evaluated when Lion comes out.
#
if(APPLE)
  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET OR "${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "")
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "force build for 64-bit Leopard" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.5" CACHE STRING "force build for 64-bit Leopard" FORCE)
    set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.5.sdk" CACHE PATH "force build for 64-bit Leopard" FORCE)
  endif()

  if(NOT "${CMAKE_OSX_SYSROOT}" STREQUAL "")
    if(NOT EXISTS "${CMAKE_OSX_SYSROOT}")
      message(FATAL_ERROR "error: CMAKE_OSX_SYSROOT='${CMAKE_OSX_SYSROOT}' does not exist")
    endif()
  endif()
endif()
