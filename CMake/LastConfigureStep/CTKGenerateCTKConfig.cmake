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

include(ctkFunctionGeneratePluginUseFile)

# Construct version numbers for CTKConfigVersion.cmake.
set(_CTK_VERSION_MAJOR ${CTK_MAJOR_VERSION})
set(_CTK_VERSION_MINOR ${CTK_MINOR_VERSION})
set(_CTK_VERSION_PATCH ${CTK_PATCH_VERSION})
# We use odd minor numbers for development versions.
# Use a date for the development patch level.
# if("${_CTK_VERSION_MINOR}" MATCHES "[13579]$")
#   include(${CTK_SOURCE_DIR}/Utilities/kwsys/kwsysDateStamp.cmake)
#   set(_CTK_VERSION_PATCH
#     "${KWSYS_DATE_STAMP_YEAR}${KWSYS_DATE_STAMP_MONTH}${KWSYS_DATE_STAMP_DAY}"
#     )
# endif()

#-----------------------------------------------------------------------------
# Settings shared between the build tree and install tree.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
set(CTK_CONFIG_INSTALL_ONLY)

# The "use" file.
set(CTK_USE_FILE ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake)

# Generate list of target to exports
set(CTK_TARGETS_TO_EXPORT ${CTK_LIBRARIES} ${CTK_PLUGIN_LIBRARIES})

# Append CTK PythonQt static libraries
if(NOT CTK_BUILD_SHARED_LIBS)
  foreach(lib ${CTK_WRAPPED_LIBRARIES_PYTHONQT})
    list(APPEND CTK_TARGETS_TO_EXPORT ${lib}PythonQt)
  endforeach()
endif()

# Export targets so they can be imported by a project using CTK
# as an external library
export(TARGETS ${CTK_TARGETS_TO_EXPORT} FILE ${CTK_SUPERBUILD_BINARY_DIR}/CTKExports.cmake)

# Generate a file containing plugin specific variables
set(CTK_PLUGIN_USE_FILE "${CTK_SUPERBUILD_BINARY_DIR}/CTKPluginUseFile.cmake")
ctkFunctionGeneratePluginUsefile(${CTK_PLUGIN_USE_FILE})

# Write a set of variables containing library specific include and library directories
set(CTK_LIBRARY_INCLUDE_DIRS_CONFIG)
foreach(lib ${CTK_LIBRARIES})
  set(${lib}_INCLUDE_DIRS ${${lib}_SOURCE_DIR} ${${lib}_BINARY_DIR})
  ctkFunctionGetIncludeDirs(${lib}_INCLUDE_DIRS ${lib})
  set(CTK_LIBRARY_INCLUDE_DIRS_CONFIG "${CTK_LIBRARY_INCLUDE_DIRS_CONFIG}
set(${lib}_INCLUDE_DIRS \"${${lib}_INCLUDE_DIRS}\")")

  ctkFunctionGetLibraryDirs(${lib}_LIBRARY_DIRS ${lib})
  set(CTK_LIBRARY_LIBRARY_DIRS_CONFIG "${CTK_LIBRARY_LIBRARY_DIRS_CONFIG}
set(${lib}_LIBRARY_DIRS \"${${lib}_LIBRARY_DIRS}\")")
endforeach()

# Determine the include directories needed.
set(CTK_INCLUDE_DIRS_CONFIG
  ${CTK_BASE_INCLUDE_DIRS}
)

set(CTKTesting_CMAKE_DIR_CONFIG "${CTKTesting_SOURCE_DIR}/CMake")

# Consider "headeronly" libraries
set(headeronly_libs CTKTesting)
foreach(lib ${headeronly_libs})
  set(${lib}_INCLUDE_DIRS ${${lib}_SOURCE_DIR} ${${lib}_BINARY_DIR})
  ctkFunctionGetIncludeDirs(${lib}_INCLUDE_DIRS ${lib})
  set(CTK_LIBRARY_INCLUDE_DIRS_CONFIG "${CTK_LIBRARY_INCLUDE_DIRS_CONFIG}
set(${lib}_INCLUDE_DIRS \"${${lib}_INCLUDE_DIRS}\")")
  set(CTK_INCLUDE_DIRS_CONFIG ${CTK_INCLUDE_DIRS_CONFIG} ${${lib}_INCLUDE_DIRS})
endforeach()



# Library directory.
set(CTK_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Plug-in output directory
if(WIN32)
  set(_plugin_output_type "RUNTIME")
else()
  set(_plugin_output_type "LIBRARY")
endif()
if(DEFINED CTK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY)
  if(IS_ABSOLUTE "${CTK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
    set(CTK_PLUGIN_LIBRARIES_DIR_CONFIG "${CTK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
  else()
    set(CTK_PLUGIN_LIBRARIES_DIR_CONFIG "${CMAKE_${_plugin_output_type}_OUTPUT_DIRECTORY}/${CTK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
  endif()
else()
  set(CTK_PLUGIN_LIBRARIES_DIR_CONFIG "${CMAKE_${_plugin_output_type}_OUTPUT_DIRECTORY}/plugins")
endif()

# External project libraries.
set(CTK_EXTERNAL_LIBRARIES_CONFIG ${CTK_EXTERNAL_LIBRARIES})

# External project library directory.
set(CTK_EXTERNAL_LIBRARY_DIRS_CONFIG ${CTK_EXTERNAL_LIBRARY_DIRS})

# Runtime library directory.
set(CTK_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Binary executable directory.
set(CTK_EXECUTABLE_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# QtDesigner plugins directory
set(CTK_QTDESIGNERPLUGINS_DIR_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# CTK external projects variables
string(REPLACE "^" ";" CTK_SUPERBUILD_EP_VARNAMES "${CTK_SUPERBUILD_EP_VARNAMES}")
set(CTK_SUPERBUILD_EP_VARS_CONFIG)
foreach(varname ${CTK_SUPERBUILD_EP_VARNAMES})
  set(CTK_SUPERBUILD_EP_VARS_CONFIG
   "${CTK_SUPERBUILD_EP_VARS_CONFIG}
set(CTK_${varname} \"${${varname}}\")")
endforeach()

# Executable locations.

# CMake extension module directory.
set(CTK_CMAKE_DIR_CONFIG ${CTK_CMAKE_DIR})
set(CTK_CMAKE_UTILITIES_DIR_CONFIG ${CTK_CMAKE_UTILITIES_DIR})

# Build configuration information.
set(CTK_CONFIGURATION_TYPES_CONFIG ${CMAKE_CONFIGURATION_TYPES})
set(CTK_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
# Configure CTKConfig.cmake for the build tree.
configure_file(${CTK_SOURCE_DIR}/CMake/CTKConfig.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfig.cmake @ONLY IMMEDIATE)
configure_file(${CTK_SOURCE_DIR}/CMake/CTKConfigVersion.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfigVersion.cmake @ONLY IMMEDIATE)
configure_file(${CTK_SOURCE_DIR}/CMake/ctkConfig.h.in
               ${CTK_CONFIG_H_INCLUDE_DIR}/ctkConfig.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

#-----------------------------------------------------------------------------
# Configure CTKConfig.cmake for the install tree.
