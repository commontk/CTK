###########################################################################
#
#  Library:   CTK
# 
#  Copyright (c) 2010  Kitware Inc.
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
###########################################################################
#
#  Program:   Visualization Toolkit
#  Module:    vtkGenerateVTKConfig.cmake
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

#
# Generate the CTKConfig.cmake file in the build tree. Also configure
# one for installation.  The file tells external projects how to use CTK.
#

# Construct version numbers for CTKConfigVersion.cmake.
SET(_CTK_VERSION_MAJOR ${CTK_MAJOR_VERSION})
SET(_CTK_VERSION_MINOR ${CTK_MINOR_VERSION})
SET(_CTK_VERSION_PATCH ${CTK_BUILD_VERSION})
# We use odd minor numbers for development versions.
# Use a date for the development patch level.
# IF("${_CTK_VERSION_MINOR}" MATCHES "[13579]$")
#   INCLUDE(${CTK_SOURCE_DIR}/Utilities/kwsys/kwsysDateStamp.cmake)
#   SET(_CTK_VERSION_PATCH
#     "${KWSYS_DATE_STAMP_YEAR}${KWSYS_DATE_STAMP_MONTH}${KWSYS_DATE_STAMP_DAY}"
#     )
# ENDIF()

#-----------------------------------------------------------------------------
# Settings shared between the build tree and install tree.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
SET(CTK_CONFIG_INSTALL_ONLY)

# The "use" file.
SET(CTK_USE_FILE ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake)

# Library directory.
SET(CTK_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Runtime library directory.
SET(CTK_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Binary executable directory.
SET(CTK_EXECUTABLE_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# QtDesigner plugins directory
SET(CTK_QTDESIGNERPLUGINS_DIR_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Determine the include directories needed.
SET(CTK_INCLUDE_DIRS_CONFIG
  ${CTK_BASE_INCLUDE_DIRS}
)

# Executable locations.

# CMake extension module directory.
SET(CTK_CMAKE_DIR_CONFIG "${CTK_CMAKE_DIR}")
SET(CTK_CMAKE_UTILITIES_DIR_CONFIG "${CTK_CMAKE_UTILITIES_DIR}")

# Build configuration information.
SET(CTK_CONFIGURATION_TYPES_CONFIG ${CMAKE_CONFIGURATION_TYPES})
SET(CTK_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
# Configure CTKConfig.cmake for the build tree.
CONFIGURE_FILE(${CTK_SOURCE_DIR}/CTKConfig.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfig.cmake @ONLY IMMEDIATE)
CONFIGURE_FILE(${CTK_SOURCE_DIR}/CTKConfigVersion.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfigVersion.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

#-----------------------------------------------------------------------------
# Configure CTKConfig.cmake for the install tree.
