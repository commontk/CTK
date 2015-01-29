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

message(STATUS "Including CMake built-in module CMakePackageConfigHelpers")
include(CMakePackageConfigHelpers OPTIONAL)
if(COMMAND configure_package_config_file)
  message(STATUS "Including CMake built-in module CMakePackageConfigHelpers - ok")
else()
  message(STATUS "Including CMake built-in module CMakePackageConfigHelpers - failed")
  message(STATUS "Including CTK module CMakePackageConfigHelpers")
  list(APPEND CMAKE_MODULE_PATH ${CTK_CMAKE_DIR}/configure_package_config_file)
  include(CMakePackageConfigHelpers)
  message(STATUS "Including CTK module CMakePackageConfigHelpers - ok")
endif()

include(ctkFunctionGeneratePluginUseFile)

set(CTKTesting_CMAKE_DIR_CONFIG "${CTKTesting_SOURCE_DIR}/CMake")


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

# CMake extension module directory.
set(CTK_CMAKE_DIR_CONFIG ${CTK_CMAKE_DIR})
set(CTK_CMAKE_UTILITIES_DIR_CONFIG ${CTK_CMAKE_UTILITIES_DIR})

# Build configuration information.
set(CTK_CONFIGURATION_TYPES_CONFIG ${CMAKE_CONFIGURATION_TYPES})
set(CTK_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
configure_file(
  ${CTK_SOURCE_DIR}/CMake/ctkConfig.h.in
  ${CTK_CONFIG_H_INCLUDE_DIR}/ctkConfig.h @ONLY
  )

install(
  FILES ${CTK_CONFIG_H_INCLUDE_DIR}/ctkConfig.h
  DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Generate a file containing plugin specific variables
set(ctk_plugin_use_file "${CTK_SUPERBUILD_BINARY_DIR}/CTKPluginUseFile.cmake")
ctkFunctionGeneratePluginUsefile(${ctk_plugin_use_file})

install(
  FILES ${ctk_plugin_use_file}
  DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development
  )

#-----------------------------------------------------------------------------
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

install(EXPORT CTKExports DESTINATION ${CTK_INSTALL_CMAKE_DIR})

#-----------------------------------------------------------------------------
# Configure 'CTKConfig.cmake' for a build tree

# CTK external projects variables
set(CTK_SUPERBUILD_EP_VARS_CONFIG)
foreach(varname ${CTK_EP_LABEL_FIND_PACKAGE_VARS} ${CTK_EP_LABEL_FIND_PACKAGE})
  set(CTK_SUPERBUILD_EP_VARS_CONFIG
   "${CTK_SUPERBUILD_EP_VARS_CONFIG}
set(${varname} \"${${varname}}\")")
endforeach()
foreach(varname ${CTK_EP_LABEL_FIND_PACKAGE})
  string(REPLACE "_DIR" "" package_name "${varname}")
  set(CTK_SUPERBUILD_EP_VARS_CONFIG
   "${CTK_SUPERBUILD_EP_VARS_CONFIG}
find_dependency(${package_name})")
endforeach()

set(CTK_CONFIG_DIR_CONFIG ${CTK_SUPERBUILD_BINARY_DIR})
set(CTK_CMAKE_DIR_CONFIG ${CTK_CMAKE_DIR})
set(CTK_CMAKE_UTILITIES_DIR_CONFIG ${CTK_CMAKE_UTILITIES_DIR})
set(CTK_CONFIG_H_INCLUDE_DIR_CONFIG ${CTK_CONFIG_H_INCLUDE_DIR})
set(CTK_EXPORT_HEADER_TEMPLATE_DIR_CONFIG ${CTK_SOURCE_DIR}/Libs)
set(CTK_LIBRARY_DIR_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(CTK_CONFIG_CODE "####### Expanded from \@CTK_CONFIG_CODE\@ #######\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# The CTK DGraph executable used to compute target dependency graphs\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_DGRAPH_EXECUTABLE \"${DGraph_EXECUTABLE}\")\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# Qt configuration\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_QT_QMAKE_EXECUTABLE \"${QT_QMAKE_EXECUTABLE}\")\n") # FIXME: Just pass Qt version (and bitness?)
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# The CTK Qt designer plugins directory\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_QTDESIGNERPLUGINS_DIR \"${CMAKE_LIBRARY_OUTPUT_DIRECTORY}\")\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# CTK library include dirctories\n")
set(_include_dirs)
foreach(lib ${CTK_LIBRARIES} CTKTesting)
  set(${lib}_INCLUDE_DIRS ${${lib}_SOURCE_DIR} ${${lib}_BINARY_DIR})
  ctkFunctionGetIncludeDirs(${lib}_INCLUDE_DIRS ${lib})
  list(APPEND _include_dirs ${${lib}_INCLUDE_DIRS})
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(${lib}_INCLUDE_DIRS \"${${lib}_INCLUDE_DIRS}\")\n")
endforeach()
list(REMOVE_DUPLICATES _include_dirs)
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_INCLUDE_DIRS \"${_include_dirs}\")\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# CTK library directories that could be used for linking\n")
foreach(lib ${CTK_LIBRARIES})
  set(${lib}_LIBRARY_DIRS "")
  ctkFunctionGetLibraryDirs(${lib}_LIBRARY_DIRS ${lib})
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(${lib}_LIBRARY_DIRS \"${${lib}_LIBRARY_DIRS}\")\n")
endforeach()
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# External project libraries\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_EXTERNAL_LIBRARIES \"${CTK_EXTERNAL_LIBRARIES}\")\n")
if(DEFINED DCMTK_HAVE_CONFIG_H_OPTIONAL AND NOT DCMTK_HAVE_CONFIG_H_OPTIONAL AND TARGET CTKDICOMCore)
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# Definition required to build DCMTK dependent libraries\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}if(\"\${CMAKE_VERSION}\" VERSION_GREATER 2.8.10)\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}  set_target_properties(CTKDICOMCore PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}endif()\n")
endif()
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}##################################################")

set(ctk_config ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfig.cmake)

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/CMake/CTKConfig.cmake.in
  ${ctk_config}
  INSTALL_DESTINATION ${CTK_SUPERBUILD_BINARY_DIR}
  PATH_VARS
    CTK_CONFIG_DIR_CONFIG
    CTK_CMAKE_DIR_CONFIG
    CTK_CMAKE_UTILITIES_DIR_CONFIG
    CTK_CONFIG_H_INCLUDE_DIR_CONFIG
    CTK_EXPORT_HEADER_TEMPLATE_DIR_CONFIG
    CTK_LIBRARY_DIR_CONFIG
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

#-----------------------------------------------------------------------------
# Configure 'CTKConfig.cmake' for an install tree

# CTK external projects. We rely on externally set
# _DIR variables or a proper CMAKE_PREFIX_PATH such
# that find_dependency/find_package can successfully
# find the external project. 
set(CTK_SUPERBUILD_EP_VARS_CONFIG)
foreach(varname ${CTK_EP_LABEL_FIND_PACKAGE})
  string(REPLACE "_DIR" "" package_name "${varname}")
  set(CTK_SUPERBUILD_EP_VARS_CONFIG
   "${CTK_SUPERBUILD_EP_VARS_CONFIG}
find_dependency(${package_name})")
endforeach()

set(CTK_CONFIG_DIR_CONFIG ${CTK_INSTALL_CMAKE_DIR})
set(CTK_CMAKE_DIR_CONFIG ${CTK_INSTALL_CMAKE_DIR})
set(CTK_CMAKE_UTILITIES_DIR_CONFIG ${CTK_INSTALL_CMAKE_DIR})
set(CTK_CONFIG_H_INCLUDE_DIR_CONFIG ${CTK_INSTALL_INCLUDE_DIR})
set(CTK_EXPORT_HEADER_TEMPLATE_DIR_CONFIG ${CTK_INSTALL_CMAKE_DIR})
set(CTK_LIBRARY_DIR_CONFIG ${CTK_INSTALL_LIB_DIR})

set(CTK_CONFIG_CODE "####### Expanded from \@CTK_CONFIG_CODE\@ #######\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# CTK library include dirctories\n")
foreach(libname ${CTK_LIBRARIES})
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(${libname}_INCLUDE_DIRS \"\${PACKAGE_PREFIX_DIR}/${CTK_INSTALL_INCLUDE_DIR}\")\n")
endforeach()
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(CTK_INCLUDE_DIRS \"\${PACKAGE_PREFIX_DIR}/${CTK_INSTALL_INCLUDE_DIR}\")\n")
set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# CTK library directories that could be used for linking\n")
foreach(lib ${CTK_LIBRARIES})
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}set(${lib}_LIBRARY_DIRS \"\")\n")
endforeach()
if(DEFINED DCMTK_HAVE_CONFIG_H_OPTIONAL AND NOT DCMTK_HAVE_CONFIG_H_OPTIONAL AND TARGET CTKDICOMCore)
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}# Definition required to build DCMTK dependent libraries\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}if(\"\${CMAKE_VERSION}\" VERSION_GREATER 2.8.10)\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}  set_target_properties(CTKDICOMCore PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})\n")
  set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}endif()\n")
endif()

set(CTK_CONFIG_CODE "${CTK_CONFIG_CODE}##################################################")

set(ctk_install_config ${CMAKE_BINARY_DIR}/CMakeFiles/CTKConfig.cmake)

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/CMake/CTKConfig.cmake.in
  ${ctk_install_config}
  INSTALL_DESTINATION ${CTK_INSTALL_CMAKE_DIR}
  PATH_VARS
    CTK_CONFIG_DIR_CONFIG
    CTK_CMAKE_DIR_CONFIG
    CTK_CMAKE_UTILITIES_DIR_CONFIG
    CTK_CONFIG_H_INCLUDE_DIR_CONFIG
    CTK_EXPORT_HEADER_TEMPLATE_DIR_CONFIG
    CTK_LIBRARY_DIR_CONFIG
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

install(
  FILES ${ctk_install_config}
  DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Configure and install 'CTKConfigVersion.cmake'
set(ctk_config_version ${CTK_SUPERBUILD_BINARY_DIR}/CTKConfigVersion.cmake)
write_basic_package_version_file(
  ${ctk_config_version}
  VERSION ${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}.${CTK_PATCH_VERSION}
  COMPATIBILITY SameMajorVersion
  )

install(
  FILES ${ctk_config_version}
  DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development
  )
