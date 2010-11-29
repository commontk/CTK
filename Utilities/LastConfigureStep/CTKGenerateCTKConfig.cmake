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

# Generate list of target to exports
SET(CTK_TARGETS_TO_EXPORT ${CTK_LIBRARIES} ${CTK_PLUGIN_LIBRARIES})

# Append CTK PythonQt static libraries
FOREACH(lib ${CTK_WRAPPED_LIBRARIES_PYTHONQT})
  LIST(APPEND CTK_TARGETS_TO_EXPORT ${lib}PythonQt)
ENDFOREACH()

# Export targets so they can be imported by a project using CTK
# as an external library
EXPORT(TARGETS ${CTK_TARGETS_TO_EXPORT} FILE ${CTK_SUPERBUILD_BINARY_DIR}/CTKExports.cmake)

# Write a set of variables containing plugin specific include directories
SET(CTK_PLUGIN_INCLUDE_DIRS_CONFIG)
FOREACH(plugin ${CTK_PLUGIN_LIBRARIES})
  SET(${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR} ${${plugin}_BINARY_DIR})
  ctkFunctionGetIncludeDirs(${plugin}_INCLUDE_DIRS ${plugin})
  SET(CTK_PLUGIN_INCLUDE_DIRS_CONFIG "${CTK_PLUGIN_INCLUDE_DIRS_CONFIG}
SET(${plugin}_INCLUDE_DIRS \"${${plugin}_INCLUDE_DIRS}\")")
ENDFOREACH()

# Determine the include directories needed.
SET(CTK_INCLUDE_DIRS_CONFIG
  ${CTK_BASE_INCLUDE_DIRS}
)

# Library directory.
SET(CTK_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# External project libraries.
SET(CTK_EXTERNAL_LIBRARIES_CONFIG ${CTK_EXTERNAL_LIBRARIES})

# External project library directory.
SET(CTK_EXTERNAL_LIBRARY_DIRS_CONFIG ${CTK_EXTERNAL_LIBRARY_DIRS})

# Runtime library directory.
SET(CTK_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Binary executable directory.
SET(CTK_EXECUTABLE_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# QtDesigner plugins directory
SET(CTK_QTDESIGNERPLUGINS_DIR_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# PythonQt directory
SET(CTK_PYTHONQT_INSTALL_DIR_CONFIG ${PYTHONQT_INSTALL_DIR})

# Executable locations.

# CMake extension module directory.
SET(CTK_CMAKE_DIR_CONFIG ${CTK_CMAKE_DIR})
SET(CTK_CMAKE_UTILITIES_DIR_CONFIG ${CTK_CMAKE_UTILITIES_DIR})

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
