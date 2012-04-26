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
# Qt is expected to be setup by CTK/CMakeLists.txt just before it includes the SuperBuild script
#

#-----------------------------------------------------------------------------
# Attempt to discover Doxygen so that DOXYGEN_EXECUTABLE is set to an appropriate default value
#
find_package(Doxygen QUIET)

#-----------------------------------------------------------------------------
# Generate cmake variable name corresponding to Libs, Plugins and Applications
#
set(ctk_libs_bool_vars)
foreach(lib ${CTK_LIBS})
  list(APPEND ctk_libs_bool_vars CTK_LIB_${lib})
endforeach()

set(ctk_plugins_bool_vars)
foreach(plugin ${CTK_PLUGINS})
  list(APPEND ctk_plugins_bool_vars CTK_PLUGIN_${plugin})
endforeach()

set(ctk_applications_bool_vars)
foreach(app ${CTK_APPS})
  list(APPEND ctk_applications_bool_vars CTK_APP_${app})
endforeach()

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#

set(ctk_cmake_boolean_args
  BUILD_TESTING
  CTK_BUILD_QTDESIGNER_PLUGINS
  CTK_USE_QTTESTING
  CTK_USE_KWSTYLE
  WITH_COVERAGE
  DOCUMENTATION_TARGET_IN_ALL
  CTEST_USE_LAUNCHERS
  CTK_WRAP_PYTHONQT_FULL
  CTK_ENABLE_Python_Wrapping
  ${ctk_libs_bool_vars}
  ${ctk_plugins_bool_vars}
  ${ctk_applications_bool_vars}
  ${ctk_lib_options_list}
  )

set(ctk_superbuild_boolean_args)
foreach(ctk_cmake_arg ${ctk_cmake_boolean_args})
  list(APPEND ctk_superbuild_boolean_args -D${ctk_cmake_arg}:BOOL=${${ctk_cmake_arg}})
endforeach()

# message("CMake boolean args:")
# foreach(arg ${ctk_superbuild_boolean_args})
#   message("  ${arg}")
# endforeach()

#-----------------------------------------------------------------------------
# Expand superbuild external project args
#
set(CTK_SUPERBUILD_EP_ARGS)
set(CTK_SUPERBUILD_EP_VARNAMES)
foreach(arg ${CTK_SUPERBUILD_EP_VARS})
  string(REPLACE ":" ";" varname_and_vartype ${arg})
  set(target_info_list ${target_info_list})
  list(GET varname_and_vartype 0 _varname)
  list(GET varname_and_vartype 1 _vartype)
  list(APPEND CTK_SUPERBUILD_EP_ARGS -D${_varname}:${_vartype}=${${_varname}})
  list(APPEND CTK_SUPERBUILD_EP_VARNAMES ${_varname})
endforeach()
string(REPLACE ";" "^" CTK_SUPERBUILD_EP_VARNAMES "${CTK_SUPERBUILD_EP_VARNAMES}")

# message("CMake external project args:")
# foreach(arg ${CTK_SUPERBUILD_EP_ARGS})
#   message("  ${arg}")
# endforeach()

#-----------------------------------------------------------------------------
# CTK Configure
#
set(proj CTK-Configure)

ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCTK_SUPERBUILD:BOOL=OFF
    -DCTK_SUPERBUILD_BINARY_DIR:PATH=${CTK_BINARY_DIR}
    ${ctk_superbuild_boolean_args}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS_INIT:STRING=${CMAKE_CXX_FLAGS_INIT}
    -DCMAKE_C_FLAGS_INIT:STRING=${CMAKE_C_FLAGS_INIT}
    -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
    -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
    -DCTK_BUILD_SHARED_LIBS:BOOL=${CTK_BUILD_SHARED_LIBS}
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    -DDOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY:PATH=${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}
    -DDOXYGEN_EXECUTABLE:FILEPATH=${DOXYGEN_EXECUTABLE}
    -DCTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCTK_INSTALL_BIN_DIR:STRING=${CTK_INSTALL_BIN_DIR}
    -DCTK_INSTALL_LIB_DIR:STRING=${CTK_INSTALL_LIB_DIR}
    -DCTK_INSTALL_QTPLUGIN_DIR:STRING=${CTK_INSTALL_QTPLUGIN_DIR}
    -DCTK_INSTALL_INCLUDE_DIR:STRING=${CTK_INSTALL_INCLUDE_DIR}
    -DCTK_INSTALL_DOC_DIR:STRING=${CTK_INSTALL_DOC_DIR}
    -DCTK_EXTERNAL_LIBRARY_DIRS:STRING=${CTK_EXTERNAL_LIBRARY_DIRS}
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    ${CTK_SUPERBUILD_EP_ARGS}
    -DCTK_SUPERBUILD_EP_VARNAMES:STRING=${CTK_SUPERBUILD_EP_VARNAMES}
  SOURCE_DIR ${CTK_SOURCE_DIR}
  BINARY_DIR ${CTK_BINARY_DIR}/CTK-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    ${CTK_DEPENDENCIES}
  )

if(CMAKE_GENERATOR MATCHES ".*Makefiles.*")
  set(ctk_build_cmd "$(MAKE)")
else()
  set(ctk_build_cmd ${CMAKE_COMMAND} --build ${CTK_BINARY_DIR}/CTK-build --config ${CMAKE_CFG_INTDIR})
endif()

#-----------------------------------------------------------------------------
# CTK
#
#message(STATUS SUPERBUILD_EXCLUDE_CTKBUILD_TARGET:${SUPERBUILD_EXCLUDE_CTKBUILD_TARGET})
if(NOT DEFINED SUPERBUILD_EXCLUDE_CTKBUILD_TARGET OR NOT SUPERBUILD_EXCLUDE_CTKBUILD_TARGET)
  set(CTKBUILD_TARGET_ALL_OPTION "ALL")
else()
  set(CTKBUILD_TARGET_ALL_OPTION "")
endif()

add_custom_target(CTK-build ${CTKBUILD_TARGET_ALL_OPTION}
  COMMAND ${ctk_build_cmd}
  WORKING_DIRECTORY ${CTK_BINARY_DIR}/CTK-build
  )
add_dependencies(CTK-build CTK-Configure)

#-----------------------------------------------------------------------------
# Custom target allowing to drive the build of CTK project itself
#
add_custom_target(CTK
  COMMAND ${ctk_build_cmd}
  WORKING_DIRECTORY ${CTK_BINARY_DIR}/CTK-build
  )
