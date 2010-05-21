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

SET(cmake_version_required "2.8")
SET(cmake_version_required_dash "2-8")

CMAKE_MINIMUM_REQUIRED(VERSION ${cmake_version_required})

# 
# CTK_KWSTYLE_EXECUTABLE
# DCMTK_DIR
# QT_QMAKE_EXECUTABLE
# VTK_DIR
# PYTHONQT_INSTALL_DIR
# PYTHON_LIBRARY
# PYTHON_INCLUDE_DIR
#

#-----------------------------------------------------------------------------
# Make sure ${CTK_BINARY_DIR}/CTK-build/bin exists
# May be used by some external project to install libs (e.g QtMobility)
IF(NOT EXISTS ${CTK_BINARY_DIR}/CTK-build/bin)
  FILE(MAKE_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
ENDIF()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
INCLUDE(ExternalProject)

SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${ep_base})

SET(ep_install_dir ${ep_base}/Install)
SET(ep_build_dir ${ep_base}/Build)
SET(ep_source_dir ${ep_base}/Source)
#SET(ep_parallelism_level)
SET(ep_build_shared_libs ON)
SET(ep_build_testing OFF)

SET(ep_common_args
  -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_TESTING:BOOL=${ep_build_testing}
  )

# Compute -G arg for configuring external projects with the same CMake generator:
IF(CMAKE_EXTRA_GENERATOR)
  SET(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
ELSE()
  SET(gen "${CMAKE_GENERATOR}")
ENDIF()

# Use this value where semi-colons are needed in ep_add args:
set(sep "^^")

#-----------------------------------------------------------------------------
# Update CMake module path
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

#-----------------------------------------------------------------------------
# Collect CTK library target dependencies
#

ctkMacroCollectAllTargetLibraries("${CTK_LIBS_SUBDIRS}" "Libs" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_PLUGINS_SUBDIRS}" "Plugins" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_APPLICATIONS_SUBDIRS}" "Applications" ALL_TARGET_LIBRARIES)
#MESSAGE(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

#-----------------------------------------------------------------------------
# Initialize NON_CTK_DEPENDENCIES variable
#
# Using the variable ALL_TARGET_LIBRARIES initialized above with the help
# of the macro ctkMacroCollectAllTargetLibraries, let's get the list of all Non-CTK dependencies.
# NON_CTK_DEPENDENCIES is expected by the macro ctkMacroShouldAddExternalProject
ctkMacroGetAllNonCTKTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_CTK_DEPENDENCIES)
#MESSAGE(STATUS NON_CTK_DEPENDENCIES:${NON_CTK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Qt is expected to be setup by CTK/CMakeLists.txt just before it includes the SuperBuild script
#

#-----------------------------------------------------------------------------
# ExternalProjects
#
SET(external_projects
  log4qt
  KWStyle
  PythonQt
  DCMTK
  ZMQ
  QtMobility
  OpenIGTLink
  VTK
  XIP
  )

# Include external projects
FOREACH(p ${external_projects})
  INCLUDE(CMakeExternals/${p}.cmake)
ENDFOREACH()
   
#-----------------------------------------------------------------------------
# CTK Utilities
#
set(proj CTK-Utilities)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    # Mandatory dependencies
    ${log4qt_DEPENDS}
    # Optionnal dependencies
    ${QtMobility_DEPENDS}
    ${kwstyle_DEPENDS}
    ${DCMTK_DEPENDS}
    ${PythonQt_DEPENDS}
    ${ZMQ_DEPENDS}
    ${OpenIGTLink_DEPENDS}
    ${VTK_DEPENDS}
    ${XIP_DEPENDS}
)

#-----------------------------------------------------------------------------
# Generate cmake variable name corresponding to Libs, Plugins and Applications
#
SET(ctk_libs_bool_vars)
FOREACH(lib ${CTK_LIBS_SUBDIRS})
  LIST(APPEND ctk_libs_bool_vars CTK_LIB_${lib})
ENDFOREACH()

SET(ctk_plugins_bool_vars)
FOREACH(plugin ${CTK_PLUGINS_SUBDIRS})
  LIST(APPEND ctk_plugins_bool_vars CTK_PLUGIN_${plugin})
ENDFOREACH()

SET(ctk_applications_bool_vars)
FOREACH(app ${CTK_APPLICATIONS_SUBDIRS})
  LIST(APPEND ctk_applications_bool_vars CTK_APP_${app})
ENDFOREACH()

#-----------------------------------------------------------------------------
# Convenient macro allowing to define superbuild arg
#
MACRO(ctk_set_superbuild_boolean_arg ctk_cmake_var)
  SET(superbuild_${ctk_cmake_var} ON)
  IF(DEFINED ${ctk_cmake_var} AND NOT ${ctk_cmake_var})
    SET(superbuild_${ctk_cmake_var} OFF)
  ENDIF()
ENDMACRO()

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#

SET(ctk_cmake_boolean_args
  BUILD_TESTING
  CTK_USE_KWSTYLE
  ${ctk_libs_bool_vars}
  ${ctk_plugins_bool_vars}
  ${ctk_applications_bool_vars}
  )

SET(ctk_superbuild_boolean_args)
FOREACH(ctk_cmake_arg ${ctk_cmake_boolean_args})
  ctk_set_superbuild_boolean_arg(${ctk_cmake_arg})
  LIST(APPEND ctk_superbuild_boolean_args -D${ctk_cmake_arg}:BOOL=${superbuild_${ctk_cmake_arg}})
ENDFOREACH()

# MESSAGE("CMake args:")
# FOREACH(arg ${ctk_superbuild_boolean_args})
#   MESSAGE("  ${arg}")
# ENDFOREACH()

#-----------------------------------------------------------------------------
# CTK Configure
#
SET(proj CTK-Configure)

ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ctk_superbuild_boolean_args}
    -DCTK_SUPERBUILD:BOOL=OFF
    -DWITH_COVERAGE:BOOL=${WITH_COVERAGE}
    -DDOCUMENTATION_TARGET_IN_ALL:BOOL=${DOCUMENTATION_TARGET_IN_ALL}
    -DDOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY:PATH=${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}
    -DCTEST_USE_LAUNCHERS:BOOL=${CTEST_USE_LAUNCHERS}
    -DCTK_SUPERBUILD_BINARY_DIR:PATH=${CTK_BINARY_DIR}
    -DCTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCTK_CXX_FLAGS:STRING=${CTK_CXX_FLAGS}
    -DCTK_C_FLAGS:STRING=${CTK_C_FLAGS}
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
	  -DZMQ_DIR:PATH=${ZMQ_DIR}                     # FindVTK expects VTK_DIR variable to be defined
	  -DOpenIGTLink_DIR:PATH=${OpenIGTLink_DIR}     # FindOpenIGTLink expects OpenIGTLink_DIR variable to be defined
    -DCTK_KWSTYLE_EXECUTABLE:FILEPATH=${CTK_KWSTYLE_EXECUTABLE}
    -DDCMTK_DIR:PATH=${DCMTK_DIR} # FindDCMTK expects DCMTK_DIR variable to be defined
    -DVTK_DIR:PATH=${VTK_DIR}     # FindVTK expects VTK_DIR variable to be defined
    -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}    # FindPythonQt expects PYTHON_INCLUDE_DIR variable to be defined
    -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}        # FindPythonQt expects PYTHON_LIBRARY variable to be defined
    -DPYTHONQT_INSTALL_DIR:PATH=${PYTHONQT_INSTALL_DIR} # FindPythonQt expects PYTHONQT_INSTALL_DIR variable to be defined
    -Dlog4qt_DIR:PATH=${log4qt_DIR} # Findlog4qt expects a log4qt_DIR variable to be defined
  SOURCE_DIR ${CTK_SOURCE_DIR}
  BINARY_DIR ${CTK_BINARY_DIR}/CTK-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    "CTK-Utilities"
  )


#-----------------------------------------------------------------------------
# CTK
#
#MESSAGE(STATUS SUPERBUILD_EXCLUDE_CTKBUILD_TARGET:${SUPERBUILD_EXCLUDE_CTKBUILD_TARGET})
IF(NOT DEFINED SUPERBUILD_EXCLUDE_CTKBUILD_TARGET OR NOT SUPERBUILD_EXCLUDE_CTKBUILD_TARGET)
  SET(proj CTK-build)
  ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    CMAKE_GENERATOR ${gen}
    SOURCE_DIR ${CTK_SOURCE_DIR}
    BINARY_DIR CTK-build
    INSTALL_COMMAND ""
    DEPENDS
      "CTK-Configure"
    )
ENDIF()

#-----------------------------------------------------------------------------
# Custom target allowing to drive the build of CTK project itself
#
ADD_CUSTOM_TARGET(CTK
  COMMAND ${CMAKE_COMMAND} --build ${CTK_BINARY_DIR}/CTK-build
  WORKING_DIRECTORY ${CTK_BINARY_DIR}/CTK-build
  )
