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
# ExternalProjects - Project should be topologically ordered
#-----------------------------------------------------------------------------
SET(external_projects
  CTKData
  Log4Qt
  KWStyle
  VTK
  PythonQt
  PythonQtGenerator # Should be added after PythonQt - See comment in CMakeExternals/PythonQtGenerator.cmake
  DCMTK
  ZMQ
  QtMobility
  QtSOAP
  qxmlrpc
  OpenIGTLink
  XIP
  ITK
  )

#-----------------------------------------------------------------------------
# WARNING - No change should be required after this comment
#           when you are adding a new external project dependency.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Declare CTK_EXTERNAL_LIBRARY_DIRS variable - This variable stores
# the library output directory associated with the different external project
# It's then used in Utilities/LastConfigureStep/CTKGenerateCTKConfig.cmake to
# configure CTKConfig.cmake.in
# This variable would then be exposed to project building against CTK
SET(CTK_EXTERNAL_LIBRARY_DIRS)

#-----------------------------------------------------------------------------
# Make sure ${CTK_BINARY_DIR}/CTK-build/bin exists
# May be used by some external project to install libs (e.g QtMobility)
IF(NOT EXISTS ${CTK_BINARY_DIR}/CTK-build/bin)
  FILE(MAKE_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
ENDIF()

#-----------------------------------------------------------------------------
# Git protocole option
#
option(CTK_USE_GIT_PROTOCOL "If behind a firewall turn this OFF to use http instead." ON)

set(git_protocol "git")
if(NOT CTK_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
INCLUDE(ExternalProject)
INCLUDE(ctkMacroEmptyExternalProject)

#SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
#SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${ep_base})

SET(ep_install_dir ${CMAKE_BINARY_DIR}/CMakeExternals/Install)
set(ep_suffix      "-cmake")

set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

# Compute -G arg for configuring external projects with the same CMake generator:
IF(CMAKE_EXTRA_GENERATOR)
  SET(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
ELSE()
  SET(gen "${CMAKE_GENERATOR}")
ENDIF()

# Use this value where semi-colons are needed in ep_add args:
set(sep "^^")

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
ctkMacroGetAllNonProjectTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_CTK_DEPENDENCIES)
#MESSAGE(STATUS NON_CTK_DEPENDENCIES:${NON_CTK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Qt is expected to be setup by CTK/CMakeLists.txt just before it includes the SuperBuild script
#

#-----------------------------------------------------------------------------
# Attempt to discover Doxygen so that DOXYGEN_EXECUTABLE is set to an appropriate default value
#
FIND_PACKAGE(Doxygen QUIET)

#-----------------------------------------------------------------------------
# Include external projects
#

SET(CTK_SUPERBUILD_EP_ARGS)

# This variable will contain the list of CMake variable specific to each external project
# that should passed to CTK.
# The item of this list should have the following form: <EP_VAR>:<TYPE>
# where '<EP_VAR>' is an external project variable and TYPE is either BOOL, PATH or FILEPATH.
# Variable appended to this list will be automatically exported in CTKConfig.cmake, prefix 'CTK_'
# will be prepended if it applied.
SET(CTK_SUPERBUILD_EP_VARS)

# This variable will contain the list of external project that CTK depends on.
SET(CTK_DEPENDS)

SET(dependency_args )
FOREACH(p ${external_projects})
  INCLUDE(CMakeExternals/${p}.cmake)
  IF(${p}_enabling_variable)
    # Provides the include and library directories either directly or provides the variable name
    # used by the corresponding Find<package>.cmake files.
    # The top-level CMakeLists.txt file will expand the variable names if not in
    # superbuild mode. The include and library dirs are then used in
    # ctkMacroBuildApp, ctkMacroBuildLib, and ctkMacroBuildPlugin
    STRING(REPLACE ";" "^" _include_dirs "${${${p}_enabling_variable}_INCLUDE_DIRS}")
    LIST(APPEND dependency_args
         -D${${p}_enabling_variable}_INCLUDE_DIRS:STRING=${_include_dirs})
    STRING(REPLACE ";" "^" _library_dirs "${${${p}_enabling_variable}_LIBRARY_DIRS}")
    LIST(APPEND dependency_args
         -D${${p}_enabling_variable}_LIBRARY_DIRS:STRING=${_library_dirs})
    IF(${${p}_enabling_variable}_FIND_PACKAGE_CMD)
      LIST(APPEND dependency_args
           -D${${p}_enabling_variable}_FIND_PACKAGE_CMD:STRING=${${${p}_enabling_variable}_FIND_PACKAGE_CMD})
    ENDIF()
  ENDIF()
  LIST(APPEND CTK_DEPENDS ${${p}_DEPENDS})
ENDFOREACH()

#MESSAGE("Superbuild args: ${dependency_args}")

# MESSAGE("CTK_DEPENDS:")
# FOREACH(dep ${CTK_DEPENDS})
#   MESSAGE("  ${dep}")
# ENDFOREACH()

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
# Set superbuild boolean args
#

SET(ctk_cmake_boolean_args
  BUILD_TESTING
  BUILD_QTDESIGNER_PLUGINS
  CTK_USE_KWSTYLE
  WITH_COVERAGE
  DOCUMENTATION_TARGET_IN_ALL
  CTEST_USE_LAUNCHERS
  CTK_WRAP_PYTHONQT_FULL
  CTK_WRAP_PYTHONQT_LIGHT
  ${ctk_libs_bool_vars}
  ${ctk_plugins_bool_vars}
  ${ctk_applications_bool_vars}
  ${ctk_lib_options_list}
  )

SET(ctk_superbuild_boolean_args)
FOREACH(ctk_cmake_arg ${ctk_cmake_boolean_args})
  LIST(APPEND ctk_superbuild_boolean_args -D${ctk_cmake_arg}:BOOL=${${ctk_cmake_arg}})
ENDFOREACH()

# MESSAGE("CMake boolean args:")
# FOREACH(arg ${ctk_superbuild_boolean_args})
#   MESSAGE("  ${arg}")
# ENDFOREACH()

#-----------------------------------------------------------------------------
# Expand superbuild external project args
#
SET(CTK_SUPERBUILD_EP_VARNAMES)
FOREACH(arg ${CTK_SUPERBUILD_EP_VARS})
  STRING(REPLACE ":" ";" varname_and_vartype ${arg})
  SET(target_info_list ${target_info_list})
  LIST(GET varname_and_vartype 0 _varname)
  LIST(GET varname_and_vartype 1 _vartype)
  LIST(APPEND CTK_SUPERBUILD_EP_ARGS -D${_varname}:${_vartype}=${${_varname}})
  LIST(APPEND CTK_SUPERBUILD_EP_VARNAMES ${_varname})
ENDFOREACH()
STRING(REPLACE ";" "^" CTK_SUPERBUILD_EP_VARNAMES "${CTK_SUPERBUILD_EP_VARNAMES}")

# MESSAGE("CMake external project args:")
# FOREACH(arg ${CTK_SUPERBUILD_EP_ARGS})
#   MESSAGE("  ${arg}")
# ENDFOREACH()

#-----------------------------------------------------------------------------
# Set CMake OSX variable to pass down the external project
set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
if(APPLE)
  list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()

#-----------------------------------------------------------------------------
# CTK Configure
#
SET(proj CTK-Configure)

ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ctk_superbuild_boolean_args}
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    -DCTK_SUPERBUILD:BOOL=OFF
    -DDOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY:PATH=${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}
    -DDOXYGEN_EXECUTABLE:FILEPATH=${DOXYGEN_EXECUTABLE}
    -DCTK_SUPERBUILD_BINARY_DIR:PATH=${CTK_BINARY_DIR}
    -DCTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCTK_INSTALL_BIN_DIR:STRING=${CTK_INSTALL_BIN_DIR}
    -DCTK_INSTALL_LIB_DIR:STRING=${CTK_INSTALL_LIB_DIR}
    -DCTK_INSTALL_INCLUDE_DIR:STRING=${CTK_INSTALL_INCLUDE_DIR}
    -DCTK_INSTALL_DOC_DIR:STRING=${CTK_INSTALL_DOC_DIR}
    -DCTK_BUILD_SHARED_LIBS:BOOL=${CTK_BUILD_SHARED_LIBS}
    -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCTK_CXX_FLAGS:STRING=${CTK_CXX_FLAGS}
    -DCTK_C_FLAGS:STRING=${CTK_C_FLAGS}
    -DCTK_EXTERNAL_LIBRARY_DIRS:STRING=${CTK_EXTERNAL_LIBRARY_DIRS}
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    ${CTK_SUPERBUILD_EP_ARGS}
    -DCTK_SUPERBUILD_EP_VARNAMES:STRING=${CTK_SUPERBUILD_EP_VARNAMES}
    ${dependency_args}
  SOURCE_DIR ${CTK_SOURCE_DIR}
  BINARY_DIR ${CTK_BINARY_DIR}/CTK-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    ${CTK_DEPENDS}
  )

IF(CMAKE_GENERATOR MATCHES ".*Makefiles.*")
  SET(ctk_build_cmd "$(MAKE)")
ELSE()
  SET(ctk_build_cmd ${CMAKE_COMMAND} --build ${CTK_BINARY_DIR}/CTK-build --config ${CMAKE_CFG_INTDIR})
ENDIF()

#-----------------------------------------------------------------------------
# CTK
#
#MESSAGE(STATUS SUPERBUILD_EXCLUDE_CTKBUILD_TARGET:${SUPERBUILD_EXCLUDE_CTKBUILD_TARGET})
IF(NOT DEFINED SUPERBUILD_EXCLUDE_CTKBUILD_TARGET OR NOT SUPERBUILD_EXCLUDE_CTKBUILD_TARGET)
  SET(CTKBUILD_TARGET_ALL_OPTION "ALL")
ELSE()
  SET(CTKBUILD_TARGET_ALL_OPTION "")
ENDIF()

ADD_CUSTOM_TARGET(CTK-build ${CTKBUILD_TARGET_ALL_OPTION}
  COMMAND ${ctk_build_cmd}
  WORKING_DIRECTORY ${CTK_BINARY_DIR}/CTK-build
  )
ADD_DEPENDENCIES(CTK-build CTK-Configure)

#-----------------------------------------------------------------------------
# Custom target allowing to drive the build of CTK project itself
#
ADD_CUSTOM_TARGET(CTK
  COMMAND ${ctk_build_cmd}
  WORKING_DIRECTORY ${CTK_BINARY_DIR}/CTK-build
  )
