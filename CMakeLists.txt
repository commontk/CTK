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

cmake_minimum_required(VERSION 2.8.4)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

set(project_policies
  CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
  CMP0002 # NEW: Logical target names must be globally unique.
  CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
  CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
  CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
  CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
  CMP0007 # NEW: List command no longer ignores empty elements.
  CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
  CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
  CMP0010 # NEW: Bad variable reference syntax is an error.
  CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
  CMP0012 # NEW: if() recognizes numbers and boolean constants.
  CMP0013 # NEW: Duplicate binary directories are not allowed.
  CMP0014 # NEW: Input directories must have CMakeLists.txt
  )
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

#-----------------------------------------------------------------------------
if(APPLE)
  # Note: By setting CMAKE_OSX_* variables before any enable_language() or project() calls,
  #       we ensure that the bitness will be properly detected.
  include(${CMAKE_SOURCE_DIR}/CMake/ctkBlockSetCMakeOSXVariables.cmake)
endif()

#-----------------------------------------------------------------------------
project(CTK)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Library mode: SHARED (default) or STATIC
#
set(CTK_LIBRARY_MODE "SHARED")

option(CTK_BUILD_SHARED_LIBS "Build CTK libraries as shared module." ON)
mark_as_advanced(CTK_BUILD_SHARED_LIBS)
if(NOT CTK_BUILD_SHARED_LIBS)
  set(CTK_LIBRARY_MODE "STATIC")
endif()

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
option(CTK_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced(CTK_SUPERBUILD)

#-----------------------------------------------------------------------------
# Output directories.
#
foreach(type LIBRARY RUNTIME ARCHIVE)
  # Make sure the directory exists
  if(DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY
     AND NOT EXISTS ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    message(FATAL_ERROR "CTK_CMAKE_${type}_OUTPUT_DIRECTORY is set to a non-existing directory [${CTK_CMAKE_${type}_OUTPUT_DIRECTORY}]")
  endif()

  if(CTK_SUPERBUILD)
    set(output_dir ${CTK_BINARY_DIR}/bin)
    if(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(CTK_CMAKE_${type}_OUTPUT_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
    endif()
  else()
    if(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(output_dir ${CTK_BINARY_DIR}/bin)
    else()
      set(output_dir ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    endif()
  endif()
  set(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
  if(NOT DEFINED CTK_PLUGIN_${type}_OUTPUT_DIRECTORY)
    set(CTK_PLUGIN_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
  endif()
endforeach()

#-----------------------------------------------------------------------------
# CTK version number.  An even minor number corresponds to releases.
#
set(CTK_MAJOR_VERSION 0)
set(CTK_MINOR_VERSION 1)
set(CTK_PATCH_VERSION 0)
set(CTK_VERSION
    "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}.${CTK_PATCH_VERSION}")

# Append the library version information to the library target
# properties.  A parent project may set its own properties and/or may
# block this.
if(NOT CTK_NO_LIBRARY_VERSION)
  set(CTK_LIBRARY_PROPERTIES ${CTK_LIBRARY_PROPERTIES}
    VERSION "${CTK_VERSION}"
    SOVERSION "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}"
    )
endif()

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
if(NOT CTK_INSTALL_BIN_DIR)
  set(CTK_INSTALL_BIN_DIR "bin")
endif()
if(NOT CTK_INSTALL_LIB_DIR)
  set(CTK_INSTALL_LIB_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
endif()
if(NOT CTK_INSTALL_INCLUDE_DIR)
  set(CTK_INSTALL_INCLUDE_DIR "include/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
endif()
if(NOT CTK_INSTALL_DOC_DIR)
  set(CTK_INSTALL_DOC_DIR "doc")
endif()

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_INCLUDE_DIRS, CTK_BASE_LIBRARIES and CTK_WRAPPED_LIBRARIES_PYTHONQT
#
set(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK base libraries" FORCE)
set(CTK_BASE_INCLUDE_DIRS CACHE INTERNAL "CTK includes" FORCE)
set(CTK_WRAPPED_LIBRARIES_PYTHONQT CACHE INTERNAL "CTK libraries wrapped using PythonQt" FORCE)

# Variable use in CTKConfig.cmake.in
set(CTK_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)
set(${PROJECT_NAME}_PLUGIN_LIBRARIES CACHE INTERNAL "CTK plugins" FORCE)

# Used by CTKGenerateCTKConfig.cmake and also used to reference script from other scripts
set(CTK_CMAKE_DIR ${CTK_SOURCE_DIR}/CMake)
set(CTK_CMAKE_UTILITIES_DIR ${CTK_SOURCE_DIR}/Utilities/CMake)

#-----------------------------------------------------------------------------
# CMake function(s) and macro(s)
#
include(CMake/ctkMacroParseArguments.cmake)
include(CMake/ctkMacroSetPaths.cmake)
include(CMake/ctkMacroListFilter.cmake)
include(CMake/ctkMacroBuildLib.cmake)
include(CMake/ctkMacroBuildLibWrapper.cmake)
include(CMake/ctkMacroBuildPlugin.cmake)
include(CMake/ctkMacroBuildApp.cmake)
include(CMake/ctkMacroCompilePythonScript.cmake)
include(CMake/ctkMacroWrapPythonQt.cmake)
include(CMake/ctkMacroSetupQt.cmake)
include(CMake/ctkMacroTargetLibraries.cmake) # Import multiple macros
include(CMake/ctkFunctionExtractOptionNameAndValue.cmake)
include(CMake/ctkMacroValidateBuildOptions.cmake)
include(CMake/ctkMacroAddCtkLibraryOptions.cmake)
include(CMake/ctkFunctionGenerateDGraphInput.cmake)
include(CMake/ctkFunctionGenerateProjectXml.cmake)
include(CMake/ctkFunctionGeneratePluginManifest.cmake)
include(CMake/ctkMacroGeneratePluginResourceFile.cmake)
include(CMake/ctkFunctionCheckCompilerFlags.cmake)
include(CMake/ctkFunctionGetIncludeDirs.cmake)
include(CMake/ctkFunctionGetLibraryDirs.cmake)
include(CMake/ctkFunctionGetGccVersion.cmake)

#-----------------------------------------------------------------------------
# Qt Designer Plugins

option(BUILD_QTDESIGNER_PLUGINS "Build Qt Designer plugins" ON)
if(BUILD_QTDESIGNER_PLUGINS)
  include(CMake/ctkMacroBuildQtDesignerPlugin.cmake)
endif()

#-----------------------------------------------------------------------------
# Testing
#
option(BUILD_TESTING "Test the project" ON)
if(BUILD_TESTING)
  enable_testing()
  include(CTest)
  set(CPP_TEST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  mark_as_advanced(TCL_TCLSH DART_ROOT)

  include(CMake/ctkMacroSimpleTest.cmake)
  include(CMake/ctkMacroSimpleTestWithData.cmake)

  # Setup file for setting custom ctest vars
  configure_file(
    CMake/CTestCustom.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
    @ONLY
    )

  # Configuration for the CMake-generated test driver
  set(CMAKE_TESTDRIVER_EXTRA_INCLUDES "#include <stdexcept>")
  set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "
    try
      {")
  set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "    }
      catch( std::exception & excp )
        {
        fprintf(stderr,\"%s\\n\",excp.what());
        return EXIT_FAILURE;
        }
      catch( ... )
        {
        printf(\"Exception caught in the test driver\\n\");
        return EXIT_FAILURE;
        }
      ")
endif()

#-----------------------------------------------------------------------------
# Coverage
#
option(WITH_COVERAGE "Enable/Disable coverage" OFF)

#-----------------------------------------------------------------------------
# Documentation
#
option(DOCUMENTATION_TARGET_IN_ALL "Include the custom target for building documentation in 'all'" OFF)
mark_as_advanced(DOCUMENTATION_TARGET_IN_ALL)

set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
 CACHE PATH "Where documentation archives should be stored")
mark_as_advanced(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)

#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
set(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
mark_as_advanced(ADDITIONAL_C_FLAGS)
set(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
mark_as_advanced(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set symbol visibility Flags
#

ctkFunctionGetGccversion(${CMAKE_CXX_COMPILER} GCC_VERSION)

# MinGW does not export all symbols automatically, so no need to set flags.
#
# With gcc < 4.5, RTTI symbols from classes declared in third-party libraries
# which are not "gcc visibility aware" are marked with hidden visibility in
# DSOs which include the class declaration and which are compiled with
# hidden visibility. This leads to dynamic_cast and exception handling problems.
# While this problem could be worked around by sandwiching the include
# directives for the third-party headers between "#pragma visibility push/pop"
# statements, it is generally safer to just use default visibility with
# gcc < 4.5.
if(CMAKE_COMPILER_IS_GNUCXX AND NOT ${GCC_VERSION} VERSION_LESS "4.5" AND NOT MINGW)
  set(VISIBILITY_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
endif()

#-----------------------------------------------------------------------------
# Set coverage Flags
#
if(WITH_COVERAGE)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(coverage_flags "-g -fprofile-arcs -ftest-coverage -O0 -DNDEBUG")
    set(COVERAGE_CXX_FLAGS ${coverage_flags})
    set(COVERAGE_C_FLAGS ${coverage_flags})
  endif()
endif()

#-----------------------------------------------------------------------------
# CTK C/CXX Flags
#
set(CTK_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
set(CTK_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings -D_FORTIFY_SOURCE=2")

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-Wl,--no-undefined" cflags)

  # With older version of gcc supporting the flag -fstack-protector-all, an extra dependency to libssp.so
  # is introduced. If gcc is smaller than 4.4.0 and the build type is Release let's not include the flag.
  # Doing so should allow to build package made for distribution using older linux distro.
  if(${GCC_VERSION} VERSION_GREATER "4.4.0" OR (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ${GCC_VERSION} VERSION_LESS "4.4.0"))
    ctkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)
  endif()
  if(MINGW)
    # suppress warnings about auto imported symbols
    set(CTK_CXX_FLAGS "-Wl,--enable-auto-import ${CTK_CXX_FLAGS}")
  endif()

  set(CTK_C_FLAGS "${cflags} ${CTK_C_FLAGS}")
  set(CTK_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${CTK_CXX_FLAGS}")
endif()

if(MSVC)
  set(msvc_suppressed_warnings
    "/wd4290" # C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
  )
  set(CTK_CXX_FLAGS "${CTK_CXX_FLAGS} ${msvc_suppressed_warnings}")
endif()

#-----------------------------------------------------------------------------
# QT
#
ctkMacroSetupQt()

# Update CTK_BASE_LIBRARIES with QT libraries
if(QT4_FOUND)
  set(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK base libraries" FORCE)
endif()

#-----------------------------------------------------------------------------
# CTK Libraries - Use ON or OFF to indicate if the library should be built by default
#
set(CTK_LIBS
  Core:ON
  PluginFramework:ON
  Widgets:OFF
  DICOM/Core:OFF
  DICOM/Widgets:OFF
  ImageProcessing/ITK/Core:OFF
  #Messaging/Core:OFF  # MessagingCore library need some love :) - Let's disable it for now :(
  #ModuleDescription:OFF # ModuleDescription library need some love :) - Let's disable it for now :(
  Scripting/Python/Core:OFF
  Scripting/Python/Widgets:OFF
  Visualization/VTK/Core:OFF
  Visualization/VTK/Widgets:OFF
  #Visualization/XIP:OFF # XIP library need some love :) - Let's disable it for now :(
  )

#-----------------------------------------------------------------------------
# CTK Plugins - Use ON or OFF to indicate if the plugin should be built by default
#
set(CTK_PLUGINS
  # Optional plug-ins implementings interfaces in PluginFramework/service/
  org.commontk.eventbus:OFF
  org.commontk.configadmin:OFF
  org.commontk.eventadmin:OFF
  org.commontk.log:OFF
  org.commontk.log4qt:OFF
  org.commontk.metatype:OFF

  # Plug-ins related to DICOM WG23 (Application Hosting)
  org.commontk.dah.core:OFF
  org.commontk.dah.app:OFF
  org.commontk.dah.host:OFF
  org.commontk.dah.exampleapp:OFF
  org.commontk.dah.examplehost:OFF

  # Misc
  org.commontk.plugingenerator.core:OFF
  org.commontk.plugingenerator.ui:OFF
  org.commontk.slicermodule:OFF
  )

#-----------------------------------------------------------------------------
# CTK Applications - Use ON or OFF to indicate if the application should be built by default
#
set(CTK_APPLICATIONS
  ctkCLIPluginExplorer:OFF
  ctkDICOM:OFF
  ctkDICOMIndexer:OFF
  ctkDICOMDemoSCU:OFF
  ctkDICOMQuery:OFF
  ctkDICOMRetrieve:OFF
  ctkDICOMQueryRetrieve:OFF
  ctkExampleHost:OFF
  ctkExampleHostedApp:OFF
  ctkEventBusDemo:OFF
  ctkPluginBrowser:OFF
  ctkPluginGenerator:OFF
  ctkDICOMObjectViewer:OFF
  ctkSimplePythonShell:OFF
  )

#-----------------------------------------------------------------------------
# To make options show up in both CTK-SuperBuild and CTK regular build, let's add them
# before the SuperBuild script is included
#

# Let's mark as advanced some default properties
mark_as_advanced(CMAKE_INSTALL_PREFIX)
mark_as_advanced(DART_TESTING_TIMEOUT)

# KWStyle
option(CTK_USE_KWSTYLE     "Enable sourcecode-based style tests." OFF)
#mark_as_advanced(CTK_USE_KWSTYLE)

#---------------------------------------------------------------------------
# Will contain a list of sub-directory without option ON or OFF
#
set(CTK_LIBS_SUBDIRS )
set(CTK_PLUGINS_SUBDIRS )
set(CTK_APPLICATIONS_SUBDIRS )

#-----------------------------------------------------------------------------
# Build options associated with CTK libraries
#
# The following FOREACH loops are used to:
#   1) Add build options associated with either libraries, plugins and applications
#   2) Update either CTK_LIBS_SUBDIRS, CTK_PLUGINS_SUBDIRS or CTK_APPS_SUBDIRS variables
#
# For CTK libraries, if the file Libs/<DIR>/<LIBNAME>/ctk_library_options.cmake exists,
# in addition to 'CTK_LIB_<DIR>/<LIBNAME>' option, the following ones
# will also be available in CMake configure menu:
#  CTK_LIB_<DIR>/<LIBNAME>_OPT1  (set to OFF)
#  CTK_LIB_<DIR>/<LIBNAME>_OPT2  (set to ON)
#
# The file Libs/<DIR>/<LIBNAME>/ctk_library_options.cmake should look like:
#
#     set(ctk_library_options
#       OPT1:OFF
#       OPT2:ON
#       )

# Build options associated with CTK libraries
set(ctk_lib_options_list) # This list will be updated in ctkFunctionExtractOptionNameAndValue
foreach(lib ${CTK_LIBS})
  ctkFunctionExtractOptionNameAndValue(${lib} lib_name lib_value)
  option(CTK_LIB_${lib_name} "Enable ${lib_name} Library." ${lib_value})
  if(CTK_LIB_${lib_name})
    ctkMacroAddCtkLibraryOptions(${lib_name})
  endif()
  list(APPEND CTK_LIBS_SUBDIRS "${lib_name}")
endforeach()

# Build options associated with CTK plugins
foreach(plugin ${CTK_PLUGINS})
  ctkFunctionExtractOptionNameAndValue(${plugin} plugin_name plugin_value)
  option(CTK_PLUGIN_${plugin_name} "Build ${plugin_name} Plugin." ${plugin_value})
  list(APPEND CTK_PLUGINS_SUBDIRS "${plugin_name}")
endforeach()

# Build options associated with CTK applications
foreach(app ${CTK_APPLICATIONS})
  ctkFunctionExtractOptionNameAndValue(${app} app_name app_value)
  option(CTK_APP_${app_name} "Build ${app_name} Application." ${app_value})
  list(APPEND CTK_APPLICATIONS_SUBDIRS "${app_name}")
endforeach()

#-----------------------------------------------------------------------------
# Generate target_directories list - List of directory corresponding to the different
# libraries, plugins and applications associated with the corresponding option name.
#

# Create list of directories corresponding to the enabled targets
set(target_directories)

foreach(lib ${CTK_LIBS_SUBDIRS})
  set(option_name CTK_LIB_${lib})
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}^^${option_name}")
endforeach()

foreach(plugin ${CTK_PLUGINS_SUBDIRS})
  set(option_name CTK_PLUGIN_${plugin})
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${plugin}^^${option_name}")
endforeach()

foreach(app ${CTK_APPLICATIONS_SUBDIRS})
  set(option_name CTK_APP_${app})
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Applications/${app}^^${option_name}")
endforeach()

#message(STATUS target_directories:${target_directories})

#-----------------------------------------------------------------------------
# Compile DGraph - An application allowing to check for cycle in DAG and also obtain the
# topological order.
try_compile(RESULT_VAR ${CTK_BINARY_DIR}/Utilities/DGraph ${CTK_SOURCE_DIR}/Utilities/DGraph
              DGraph
              CMAKE_FLAGS
                -DQT_QMAKE_EXECUTABLE:FILE=${QT_QMAKE_EXECUTABLE}
                -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE
              OUTPUT_VARIABLE output)
if(NOT RESULT_VAR)
  message(FATAL_ERROR "Failed to compile DGraph application.\n${output}")
endif()
find_program(DGraph_EXECUTABLE DGraph
  "${CTK_BINARY_DIR}/Utilities/DGraph/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/bin/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Debug/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Release/")
mark_as_advanced(DGraph_EXECUTABLE)

#-----------------------------------------------------------------------------
# Let's make sure the enabled/disabled libraries, plugins or applications are coherent
#
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}")
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_EXTERNALS)
ctkMacroValidateBuildOptions("${CTK_BINARY_DIR}" "${DGraph_EXECUTABLE}" "${target_directories}")

#-----------------------------------------------------------------------------
# CTK Python wrapping - Propose the option only if CTK_LIB_Scriping/Python/Core is ON
if(CTK_LIB_Scripting/Python/Core)
  find_package(PythonInterp)
  if(NOT PYTHONINTERP_FOUND)
    message(SEND_ERROR "PYTHON_EXECUTABLE variable should be set to build CTK_LIB_Scripting/Python")
  endif()
  find_package(PythonLibs)
  if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR "PYTHON_LIBRARIES and PYTHON_INCLUDE_DIRS should be set to build CTK_LIB_Scripting/Python")
  endif()
  option(CTK_WRAP_PYTHONQT_FULL "Experimental - Wrap CTK classes using Qt meta-object system into Python language" OFF)
  mark_as_advanced(CTK_WRAP_PYTHONQT_FULL)
  option(CTK_WRAP_PYTHONQT_LIGHT "Wrap CTK classes using Qt meta-object system into Python language" OFF)
  if(CTK_WRAP_PYTHONQT_FULL AND CTK_WRAP_PYTHONQT_LIGHT)
    message(SEND_ERROR "CTK_WRAP_PYTHONQT_{LIGHT,FULL} options are mutually exclusive. Please enable only one !")
  endif()
else()
  # Note that an unset boolean variable is considered to be False
  unset(CTK_WRAP_PYTHONQT_FULL CACHE)
  unset(CTK_WRAP_PYTHONQT_LIGHT CACHE)
endif()

#-----------------------------------------------------------------------------
# DGraph
#

# Generate DGraph input file expected by DGraph
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_OPTION)

# Obtain list of target ordered topologically
ctkMacroSetPaths("${QT_INSTALLED_LIBRARY_DIR}")
execute_process(
  COMMAND "${DGraph_EXECUTABLE}" "${CTK_BINARY_DIR}/DGraphInput.txt"
  WORKING_DIRECTORY ${CTK_BINARY_DIR}
  RESULT_VARIABLE RESULT_VAR
  OUTPUT_VARIABLE CTEST_PROJECT_SUBPROJECTS_OUTPUT
  ERROR_VARIABLE error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
if(RESULT_VAR)
  message(FATAL_ERROR "Failed to obtain list of target ordered topologically.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
endif()

# Convert 'CTEST_PROJECT_SUBPROJECTS_OUTPUT' to a list
string(REPLACE " " "\\;" CTEST_PROJECT_SUBPROJECTS "${CTEST_PROJECT_SUBPROJECTS_OUTPUT}")
set(CTEST_PROJECT_SUBPROJECTS ${CTEST_PROJECT_SUBPROJECTS})

# If the list of subproject is empty, let's at least build CTKCore
list(LENGTH CTEST_PROJECT_SUBPROJECTS subproject_count)
IF (subproject_count EQUAL 0)
  set(CTEST_PROJECT_SUBPROJECTS CTKCore)
endif()

# Configure CTestConfigSubProject.cmake used that could be used by CTest scripts
configure_file(${CTK_SOURCE_DIR}/CTestConfigSubProject.cmake.in
               ${CTK_BINARY_DIR}/CTestConfigSubProject.cmake)

#-----------------------------------------------------------------------------
# Project.xml
#

# Generate Project.xml file expected by the CTest driver script
ctkFunctionGenerateProjectXml(${CTK_BINARY_DIR} ${PROJECT_NAME} "${target_directories}" ${CTK_SUPERBUILD})

#-----------------------------------------------------------------------------
# CTK dependencies - Projects should be TOPOLOGICALLY ordered
#-----------------------------------------------------------------------------
set(CTK_POSSIBLE_DEPENDENCIES
  CTKData
  Log4Qt
  KWStyle
  VTK
  PythonQt
  PythonQtGenerator # Should be added after PythonQt - See comment in CMakeExternals/PythonQtGenerator.cmake
  DCMTK
  ZMQ
  QtSOAP
  qxmlrpc
  OpenIGTLink
  XIP
  ITK
  )
set(CTK_DEPENDENCIES) # This variable will contain the list of required CTK dependencies
include(CMake/ctkBlockCheckDependencies.cmake)

#-----------------------------------------------------------------------------
# Superbuild script
#

if(CTK_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

#-----------------------------------------------------------------------------
# Expand variables containing include and library directories for external projects
# This relies on the variable EXTERNAL_TARGETS set in ctkMacroValidateBuildOptions
foreach(_external_target ${EXTERNAL_TARGETS})
  if(${_external_target}_FIND_PACKAGE_CMD)
    #message("Calling find_package(${${_external_target}_FIND_PACKAGE_CMD})")
    find_package(${${_external_target}_FIND_PACKAGE_CMD} REQUIRED)
  endif()
endforeach()

foreach(_external_target ${EXTERNAL_TARGETS})
  if(${_external_target}_INCLUDE_DIRS)
    #message("[${_external_target}] Resolving include variables: ${${_external_target}_INCLUDE_DIRS}")
    foreach(_include_variable ${${_external_target}_INCLUDE_DIRS})
      #message("[${_external_target}] Appending ${${_include_variable}}")
      if(${_include_variable})
        list(APPEND ${_external_target}_INCLUDE_DIRS ${${_include_variable}})
      else()
        list(APPEND ${_external_target}_INCLUDE_DIRS ${_include_variable})
      endif()
      #message("[${_external_target}] New dirs: ${${_external_target}_INCLUDE_DIRS}")
    endforeach()
    #message("[${_external_target}] Appended dirs: ${${_external_target}_INCLUDE_DIRS}")
  endif()
  if(${_external_target}_LIBRARY_DIRS)
    #message("[${_external_target}] Resolving library variables: ${${_external_target}_LIBRARY_DIRS}")
    foreach(_library_variable ${${_external_target}_LIBRARY_DIRS})
      #message("[${_external_target}] Appending ${${_library_variable}}")
      if(${_library_variable})
        list(APPEND ${_external_target}_LIBRARY_DIRS ${${_library_variable}})
      else()
        list(APPEND ${_external_target}_LIBRARY_DIRS ${_library_variable})
      endif()
      #message("[${_external_target}] New dirs: ${${_external_target}_LIBRARY_DIRS}")
    endforeach()
    #message("[${_external_target}] Appended dirs: ${${_external_target}_LIBRARY_DIRS}")
  endif()
endforeach()

# Since PYTHONQT_USE_VTK library option can be enabled independently of
# Visualization/VTK/Core, let's make sure VTK has been properly discovered
if(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  find_package(VTK REQUIRED)
endif()
set(CTK_WRAP_PYTHONQT_USE_VTK ${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK})

#-----------------------------------------------------------------------------
# CTK_SUPERBUILD_BINARY_DIR

# If CTK_SUPERBUILD_BINARY_DIR isn't defined, it means CTK is *NOT* build using Superbuild.
# In that specific case, CTK_SUPERBUILD_BINARY_DIR should default to CTK_BINARY_DIR
if(NOT DEFINED CTK_SUPERBUILD_BINARY_DIR)
  set(CTK_SUPERBUILD_BINARY_DIR ${CTK_BINARY_DIR})
endif()

#-----------------------------------------------------------------------------
# Configure files with settings
#
configure_file(${CTK_SOURCE_DIR}/UseCTK.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake COPYONLY IMMEDIATE)

set(CTK_CONFIG_H_INCLUDE_DIR ${CTK_BINARY_DIR})

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
set(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
set(CMAKE_C_FLAGS ${CTK_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

#-----------------------------------------------------------------------------
# Set the header template which defines custom export/import macros
# for shared libraries
#
set(CTK_EXPORT_HEADER_TEMPLATE "${CTK_SOURCE_DIR}/Libs/ctkExport.h.in")

#-----------------------------------------------------------------------------
# Setup testing environment before Libs are added
#
if(BUILD_TESTING)
  add_subdirectory(Libs/Testing)
endif()

#-----------------------------------------------------------------------------
# Add CTK library subdirectories
#
foreach(lib ${CTK_LIBS_SUBDIRS})
  IF (CTK_LIB_${lib})
    add_subdirectory(Libs/${lib})
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
foreach(plugin ${CTK_PLUGINS_SUBDIRS})
  if(CTK_PLUGIN_${plugin})
    add_subdirectory(Plugins/${plugin})
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
foreach(app ${CTK_APPLICATIONS_SUBDIRS})
  IF (CTK_APP_${app})
    add_subdirectory(Applications/${app})
  endif()
endforeach()

if(BUILD_TESTING)
  add_subdirectory(Applications/Testing)
endif()


#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
#add_subdirectory(Testing)
#add_subdirectory(Examples)

#-----------------------------------------------------------------------------
# Style Checking configuration
#
include(Utilities/KWStyle/KWStyle.cmake)

#---------------------------------------------------------------------------
# Documentation
#
add_subdirectory( Documentation )

#-----------------------------------------------------------------------------
# The commands in this directory are intended to be executed as
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
add_subdirectory(Utilities/LastConfigureStep)

