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
###########################################################################
#
#  Program:   Visualization Toolkit
#  Module:    Utilities/LastConfigureStep/CMakeLists.txt
#
#  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
#
#  All rights reserved.
#  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
#
#     This software is distributed WITHOUT ANY WARRANTY; without even
#     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#     PURPOSE.  See the above copyright notice for more information.
#
###########################################################################

# The commands in this directory are intended to be executed at
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
# It enable the above commands to use variables that might have been configured
# in previous SUBDIRS. This is especially important when it comes to
# the CONFIGURE_FILE command, since in IMMEDIATE mode that command will
# use the current values of CMake variables instead of waiting until the
# end of CMakeLists processing, i.e. instead of waiting until some variables
# are configured in SUBDIRS.


set(ctk_target_dependencies)

# Loop over the list of CTK* targets and retrieve the associated dependencies
foreach(ctk_target ${CTEST_PROJECT_SUBPROJECTS})
  list(APPEND ctk_target_dependencies ${${ctk_target}_LIB_DEPENDS})
endforeach()


set(CTK_EXTERNAL_LIBRARIES)

# Loop over dependencies and append to CTK_EXTERNAL_LIBRARIES
# target dependency which are neither CTK or static library
set(link_type)
foreach(ctk_target_dependency ${ctk_target_dependencies})
  string(REGEX MATCH "^(general|optimized|debug)$" is_link_type ${ctk_target_dependency})
  if(is_link_type)
    set(link_type ${ctk_target_dependency})
  else()
    # Sanity checks - link_type shouldn't be empty
    if(link_type STREQUAL "")
      message(SEND_ERROR "link_type shouldn't be empty")
    endif()
    # Make sure ctk_target_dependency is not a CTK library
    string(REGEX MATCH "(^(c|C)(t|T)(k|K)[a-zA-Z0-9]+$)" is_ctk_library ${ctk_target_dependency})
    if(NOT is_ctk_library)
      # Make sure ctk_target_dependency is not a static library
      get_filename_component(ctk_target_dependency_ext ${ctk_target_dependency} EXT)
      if(NOT ctk_target_dependency_ext STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX)
        list(APPEND CTK_EXTERNAL_LIBRARIES ${link_type} ${ctk_target_dependency})
      endif()
      set(link_type)
    endif()
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Create the CTKConfig.cmake file containing the CTK configuration.
# Since it might generate configuration file depending
include(CTKGenerateCTKConfig.cmake)
