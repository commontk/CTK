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

cmake_minimum_required(VERSION 2.8.9)

foreach(p
  CMP0054 # CMake 3.1
  )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Superbuild
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(EXTERNAL_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMakeExternals)
set(EXTERNAL_PROJECT_FILE_PREFIX "")
include(ExternalProject)
include(ctkMacroCheckExternalProjectDependency)

#-----------------------------------------------------------------------------
if(APPLE)
  # Note: By setting CMAKE_OSX_* variables before any enable_language() or project() calls,
  #       we ensure that the bitness will be properly detected.
  include(ctkBlockSetCMakeOSXVariables)
  mark_as_superbuild(
    VARS CMAKE_OSX_ARCHITECTURES:STRING CMAKE_OSX_SYSROOT:PATH CMAKE_OSX_DEPLOYMENT_TARGET:STRING
    ALL_PROJECTS
    )
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
mark_as_superbuild(CTK_BUILD_SHARED_LIBS)
if(NOT CTK_BUILD_SHARED_LIBS)
  set(CTK_LIBRARY_MODE "STATIC")
endif()

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
if(NOT CMAKE_CONFIGURATION_TYPES)
  mark_as_superbuild(VARS CMAKE_BUILD_TYPE ALL_PROJECTS)
endif()

mark_as_superbuild(
  VARS
    CMAKE_PREFIX_PATH:STRING
    CMAKE_DEBUG_POSTFIX:STRING
  ALL_PROJECTS
  )

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
option(CTK_SUPERBUILD "Build ${PROJECT_NAME} and the projects it depends on." ON)
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
    mark_as_superbuild(CTK_CMAKE_${type}_OUTPUT_DIRECTORY:PATH)
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
if(NOT CTK_INSTALL_PLUGIN_DIR)
  set(CTK_INSTALL_PLUGIN_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}/plugins")
endif()
if(NOT CTK_INSTALL_CMAKE_DIR)
  set(CTK_INSTALL_CMAKE_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}/CMake")
endif()
if(NOT CTK_INSTALL_INCLUDE_DIR)
  set(CTK_INSTALL_INCLUDE_DIR "include/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
endif()
if(NOT CTK_INSTALL_PLUGIN_INCLUDE_DIR)
  set(CTK_INSTALL_PLUGIN_INCLUDE_DIR ${CTK_INSTALL_INCLUDE_DIR})
endif()
if(NOT CTK_INSTALL_QTPLUGIN_DIR)
  set(CTK_INSTALL_QTPLUGIN_DIR ${CTK_INSTALL_LIB_DIR})
endif()
if(NOT CTK_INSTALL_DOC_DIR)
  set(CTK_INSTALL_DOC_DIR "doc")
endif()

mark_as_superbuild(
  CTK_INSTALL_BIN_DIR:STRING
  CTK_INSTALL_LIB_DIR:STRING
  CTK_INSTALL_PLUGIN_DIR:STRING
  CTK_INSTALL_CMAKE_DIR:STRING
  CTK_INSTALL_INCLUDE_DIR:STRING
  CTK_INSTALL_PLUGIN_INCLUDE_DIR:STRING
  CTK_INSTALL_QTPLUGIN_DIR:STRING
  CTK_INSTALL_DOC_DIR:STRING
  )

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_LIBRARIES and CTK_WRAPPED_LIBRARIES_PYTHONQT
#
set(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK base libraries" FORCE)
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
foreach(file
  CMake/ctkListToString.cmake
  CMake/ctkMacroParseArguments.cmake
  CMake/ctkMacroSetPaths.cmake
  CMake/ctkMacroListFilter.cmake
  CMake/ctkMacroOptionUtils.cmake
  CMake/ctkMacroBuildLib.cmake
  CMake/ctkMacroBuildLibWrapper.cmake
  CMake/ctkMacroBuildPlugin.cmake
  CMake/ctkMacroBuildApp.cmake
  CMake/ctkMacroBuildQtPlugin.cmake
  CMake/ctkMacroCompilePythonScript.cmake
  CMake/ctkMacroGenerateMocs.cmake
  CMake/ctkMacroWrapPythonQt.cmake
  CMake/ctkMacroSetupQt.cmake
  CMake/ctkMacroTargetLibraries.cmake # Import multiple macros
  CMake/ctkFunctionExtractOptionNameAndValue.cmake
  CMake/ctkMacroValidateBuildOptions.cmake
  CMake/ctkMacroAddCtkLibraryOptions.cmake
  CMake/ctkFunctionGenerateDGraphInput.cmake
  CMake/ctkFunctionGeneratePluginManifest.cmake
  CMake/ctkMacroGeneratePluginResourceFile.cmake
  CMake/ctkFunctionAddPluginRepo.cmake
  CMake/ctkFunctionCheckCompilerFlags.cmake
  CMake/ctkFunctionCheckoutRepo.cmake
  CMake/ctkFunctionGetIncludeDirs.cmake
  CMake/ctkFunctionGetLibraryDirs.cmake
  CMake/ctkFunctionGetGccVersion.cmake
  CMake/ctkFunctionGetCompilerVisibilityFlags.cmake
  CMake/ctkFunctionCompileSnippets.cmake
  )
  include(${file})
  install(FILES ${file} DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development)
endforeach()

foreach(file
  Libs/ctkExport.h.in
  CMake/ctkLinkerAsNeededFlagCheck.cmake
  CMake/ctk_compile_python_scripts.cmake.in
  CMake/CMakeFindDependencyMacro.cmake
  )
  install(FILES ${file} DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development)
endforeach()

install(FILES
  CMake/ctkLinkerAsNeededFlagCheck/CMakeLists.txt
  CMake/ctkLinkerAsNeededFlagCheck/A.cpp
  CMake/ctkLinkerAsNeededFlagCheck/B.cpp
  CMake/ctkLinkerAsNeededFlagCheck/C.cpp
  DESTINATION ${CTK_INSTALL_CMAKE_DIR}/ctkLinkerAsNeededFlagCheck
  COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Testing
#
include(CTest)
mark_as_advanced(BUILD_TESTING)
mark_as_superbuild(BUILD_TESTING)

if(BUILD_TESTING)
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
# QtTesting
#
option(CTK_USE_QTTESTING "Enable/Disable QtTesting" OFF)
mark_as_advanced(CTK_USE_QTTESTING)
mark_as_superbuild(CTK_USE_QTTESTING)

#-----------------------------------------------------------------------------
# Coverage
#
option(WITH_COVERAGE "Enable/Disable coverage" OFF)
mark_as_advanced(WITH_COVERAGE)
mark_as_superbuild(WITH_COVERAGE)

#-----------------------------------------------------------------------------
# Documentation
#
option(DOCUMENTATION_TARGET_IN_ALL "Include the custom target for building documentation in 'all'" OFF)
mark_as_advanced(DOCUMENTATION_TARGET_IN_ALL)
mark_as_superbuild(DOCUMENTATION_TARGET_IN_ALL)

set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
 CACHE PATH "Where documentation archives should be stored")
mark_as_advanced(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)
mark_as_superbuild(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)

# Attempt to discover Doxygen so that DOXYGEN_EXECUTABLE is set to an appropriate default value
find_package(Doxygen QUIET)
mark_as_superbuild(DOXYGEN_EXECUTABLE)

#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
set(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
mark_as_advanced(ADDITIONAL_C_FLAGS)
mark_as_superbuild(ADDITIONAL_C_FLAGS)

set(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
mark_as_advanced(ADDITIONAL_CXX_FLAGS)
mark_as_superbuild(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set symbol visibility Flags
#

ctkFunctionGetCompilerVisibilityFlags(VISIBILITY_CXX_FLAGS)

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
  set(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings")
  if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(cflags "${cflags} -D_FORTIFY_SOURCE=2")
  endif()

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-Wl,--no-undefined" cflags)

  ctkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)

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
  set(CTK_CXX_FLAGS "${CTK_CXX_FLAGS} ${msvc_suppressed_warnings} -DNOMINMAX")
endif()

#-----------------------------------------------------------------------------
# Check if the linker will resolve symbols of underlinked libraries
#
if(UNIX AND NOT APPLE)
  include(ctkLinkerAsNeededFlagCheck)
  if(CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED)
    set(CTK_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
  endif()
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
# To make options show up in both CTK-SuperBuild and CTK regular build, let's add them
# before the SuperBuild script is included
#

#-----------------------------------------------------------------------------
# Special "BUILD ALL" options

# Build all CTK plug-ins
option(CTK_BUILD_ALL_PLUGINS "Build all CTK plug-ins" OFF)
mark_as_superbuild(CTK_BUILD_ALL_PLUGINS)

# Build all CTK libraries
option(CTK_BUILD_ALL_LIBRARIES "Build all CTK libraries" OFF)
mark_as_superbuild(CTK_BUILD_ALL_LIBRARIES)

# Build all CTK applications
option(CTK_BUILD_ALL_APPS "Build all CTK applications" OFF)
mark_as_superbuild(CTK_BUILD_ALL_APPS)

# Build everything
option(CTK_BUILD_ALL "Build everything in CTK" OFF)
mark_as_superbuild(CTK_BUILD_ALL)
if(CTK_BUILD_ALL)
  set(CTK_BUILD_ALL_PLUGINS 1)
  set(CTK_BUILD_ALL_LIBRARIES 1)
  set(CTK_BUILD_ALL_APPS 1)
endif()

#-----------------------------------------------------------------------------
# Other options

# Git protocol option
option(CTK_USE_GIT_PROTOCOL "If behind a firewall turn this OFF to use http instead." ON)
mark_as_advanced(CTK_USE_GIT_PROTOCOL)
set(git_protocol "git")
if(NOT CTK_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

# Let's mark as advanced some default properties
mark_as_advanced(CMAKE_INSTALL_PREFIX)
mark_as_advanced(DART_TESTING_TIMEOUT)

# KWStyle
option(CTK_USE_KWSTYLE     "Enable sourcecode-based style tests." OFF)
mark_as_advanced(CTK_USE_KWSTYLE)
mark_as_superbuild(CTK_USE_KWSTYLE)

# Qt Designer Plugins
option(CTK_BUILD_QTDESIGNER_PLUGINS "Build Qt Designer plugins" ON)
mark_as_advanced(CTK_BUILD_QTDESIGNER_PLUGINS)
mark_as_superbuild(CTK_BUILD_QTDESIGNER_PLUGINS)

#-----------------------------------------------------------------------------
# CTK Libraries
#

ctk_lib_option(Core
               "Build the Core library" ON)

ctk_lib_option(PluginFramework
               "Build the Plugin Framework" OFF
               CTK_ENABLE_PluginFramework)

ctk_lib_option(Widgets
               "Build the Widgets library" OFF
               CTK_ENABLE_Widgets OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES) OR CTK_USE_QTTESTING)

ctk_lib_option(DICOM/Core
               "Build the DICOM Core library" OFF
               CTK_ENABLE_DICOM OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES))

ctk_lib_option(DICOM/Widgets
               "Build the DICOM Widgets library" OFF
               CTK_ENABLE_DICOM AND CTK_ENABLE_Widgets OR (CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES))

ctk_lib_option(ImageProcessing/ITK/Core
               "Build the ITK Core library" OFF)

ctk_lib_option(Scripting/Python/Core
               "Build the Python Core library" OFF
               CTK_ENABLE_Python_Wrapping)

ctk_lib_option(Scripting/Python/Widgets
               "Build the Python Widgets library" OFF)

# VTK libraries have not yet been tested with Qt5
if(CTK_QT_VERSION VERSION_LESS "5")
ctk_lib_option(Visualization/VTK/Core
               "Build the VTK Core library" OFF)

ctk_lib_option(Visualization/VTK/Widgets
               "Build the VTK Widgets library" OFF)
endif()

ctk_lib_option(CommandLineModules/Core
               "Build the Command Line Module core library" OFF)

ctk_lib_option(CommandLineModules/Frontend/QtWebKit
               "Build the QtWebKit based Command Line Module front-end" OFF)

ctk_lib_option(CommandLineModules/Frontend/QtGui
               "Build the QtGui based Command Line Module front-end" OFF)

ctk_lib_option(CommandLineModules/Backend/XMLChecker
               "Build the Command Line Module back-end for checking XML" OFF)

ctk_lib_option(CommandLineModules/Backend/LocalProcess
               "Build the Command Line Module back-end for local processes" OFF)

ctk_lib_option(CommandLineModules/Backend/FunctionPointer
               "Build the Command Line Module back-end for function pointers" OFF)

ctk_lib_option(XNAT/Core
               "Build the XNAT Core library" OFF)

ctk_lib_option(XNAT/Widgets
               "Build the XNAT Widgets library" OFF)

#ctk_lib_option(Visualization/XIP
#               "Build the XIP library" OFF)

# Save the set of enabled libs in a cache file
set(_enabled_libs)
foreach(_lib ${CTK_LIBS})
  if(CTK_LIB_${_lib})
    list(APPEND _enabled_libs ${_lib})
  endif()
endforeach()
set(CTK_ENABLED_LIBS ${_enabled_libs} CACHE INTERNAL "" FORCE)
#-----------------------------------------------------------------------------
# CTK Applications - Use ON or OFF to indicate if the application should be built by default
#

ctk_app_option(ctkDICOM
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)


ctk_app_option(ctkDICOM2
               "Build the new DICOM example application (experimental)" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMIndexer
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMDemoSCU
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMQuery
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMRetrieve
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMQueryRetrieve
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMHost
               "Build the DICOM application host example application" OFF
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkExampleHost
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkExampleHostedApp
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOMApplicationHosting AND CTK_BUILD_EXAMPLES)

if(CTK_QT_VERSION VERSION_LESS "5")
ctk_app_option(ctkEventBusDemo
               "Build the DICOM example application" OFF
               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)
endif()

ctk_app_option(ctkCommandLineModuleExplorer
               "Build the Command Line Module Explorer" OFF
               CTK_BUILD_EXAMPLES)

# We use the CTKWidgets library together with the Qt Designer plug-in
# in ctkCommandLineModuleExplorer, so enabling the options here.
# (We do not need to link them into the executable, hence no entries
# in target_libraries.cmake)

if(CTK_APP_ctkCommandLineModuleExplorer)
  foreach(_option CTK_BUILD_QTDESIGNER_PLUGINS)
    if(NOT ${_option})
      get_property(_docstring CACHE ${_option} PROPERTY HELPSTRING)
      set(${_option} ON CACHE BOOL "${_docstring}" FORCE)
      message("Enabling option [${_option}] required by [ctkCommandLineModuleExplorer]")
    endif()
  endforeach()
endif()

ctk_app_option(ctkPluginBrowser
               "Build the DICOM example application" OFF
               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkPluginGenerator
               "Build the DICOM example application" OFF
               CTK_ENABLE_PluginFramework AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkDICOMObjectViewer
               "Build the DICOM example application" OFF
               CTK_ENABLE_DICOM AND CTK_BUILD_EXAMPLES)

ctk_app_option(ctkSimplePythonShell
               "Build the DICOM example application" OFF
               CTK_ENABLE_Python_Wrapping AND CTK_BUILD_EXAMPLES)

if(CTK_USE_QTTESTING AND CTK_QT_VERSION VERSION_LESS "5")
  ctk_app_option(ctkQtTesting
                "Build the ctkQtTesting example application" OFF
                CTK_BUILD_EXAMPLES)
endif()

ctk_app_option(ctkXnatTreeBrowser
               "Build the XNAT Tree Browser application" OFF
               CTK_BUILD_EXAMPLES)

# Save the set of enabled apps in a cache file
set(_enabled_apps)
foreach(_app ${CTK_APPS})
  if(CTK_APP_${_app})
    list(APPEND _enabled_apps ${_app})
  endif()
endforeach()
set(CTK_ENABLED_APPS ${_enabled_apps} CACHE INTERNAL "" FORCE)

#-----------------------------------------------------------------------------
# CTK Plugins - none of them is build by default
#

# Plugins in the list below are not build by default
set(plugin_list
  # Optional plug-ins implementings interfaces in PluginFramework/service/
  org.commontk.configadmin
  org.commontk.eventadmin
  org.commontk.log
  org.commontk.metatype
  )

if(CTK_QT_VERSION VERSION_LESS "5")
  list(APPEND plugin_list org.commontk.log4qt)
endif()

foreach(_plugin ${plugin_list})
  ctk_plugin_option(${_plugin} "Build the ${_plugin} plugin." OFF)
endforeach()

# Plug-ins related to the PluginGenerator application
ctk_plugin_option(org.commontk.plugingenerator.core "Build the org.commontk.plugingenerator.core plugin." OFF)
ctk_plugin_option(org.commontk.plugingenerator.ui
                  "Build the org.commontk.plugingenerator.ui plugin." OFF
                  CTK_APP_ctkPluginGenerator)

# Plug-ins related to DICOM WG23 (Application Hosting)
ctk_plugin_option(org.commontk.dah.core "Build the org.commontk.dah.core plugin." OFF)
ctk_plugin_option(org.commontk.dah.hostedapp "Build the org.commontk.dah.hostedapp plugin." OFF
                  CTK_ENABLE_DICOMApplicationHosting)
ctk_plugin_option(org.commontk.dah.host "Build the org.commontk.dah.host plugin." OFF
                  CTK_ENABLE_DICOMApplicationHosting)

ctk_plugin_option(org.commontk.dah.exampleapp
                  "Build the org.commontk.dah.exampleapp plugin." OFF
                  CTK_APP_ctkExampleHostedApp)

ctk_plugin_option(org.commontk.dah.cmdlinemoduleapp
                  "Build the org.commontk.dah.cmdlinemoduleapp plugin." OFF
                  CTK_APP_ctkCommandLineModuleApp)

ctk_plugin_option(org.commontk.dah.examplehost
                  "Build the org.commontk.dah.examplehost plugin." OFF
                  CTK_APP_ctkExampleHost)

# Plug-ins related to the EventBus demo application
if(CTK_QT_VERSION VERSION_LESS "5")
ctk_plugin_option(org.commontk.eventbus
                  "Build the org.commontk.eventbus plugin." OFF
                  CTK_APP_ctkEventBusDemo)
endif()

# Add the PluginsContrib repo to the build system
option(CTK_USE_CONTRIBUTED_PLUGINS OFF "Use CTK plug-ins from the PluginsContrib repository")
mark_as_advanced(CTK_USE_CONTRIBUTED_PLUGINS)
mark_as_superbuild(CTK_USE_CONTRIBUTED_PLUGINS)
if(CTK_USE_CONTRIBUTED_PLUGINS)
  ctkFunctionAddPluginRepo(NAME PluginsContrib
                           GIT_URL github.com/commontk/PluginsContrib.git
                           GIT_TAG f016d35
                          )
  mark_as_superbuild(PluginsContrib_DIR:PATH)
endif()

#-----------------------------------------------------------------------------
# High-Level CTK options

# The ctk_enable_option macro expects a logical expression after the first
# three arguments. This expression should only contain build option names
# which belong to leafs in the required dependency tree.

# The CTK infrastructure for high-level DICOM support. This includes
# DICOM indexing, networking, and GUI widgets.
ctk_enable_option(DICOM "Enable default DICOM support" OFF
                  CTK_LIB_DICOM/Widgets)

# The CTK infrastructure for building a DICOM Part 19 compliant application
# host and/or hosted application. This will not enable any example plugins
# or executables (enable CTK_ENABLE_EXAMPLES for that).
ctk_enable_option(DICOMApplicationHosting "Enable DICOM Part 19 Application Hosting support" OFF
                  CTK_PLUGIN_org.commontk.dah.host AND CTK_PLUGIN_org.commontk.dah.hostedapp)

# The CTK Qt Widgets. This will enable the Qt Widget library only.
# It might trigger the enabling of other widget libraries in combination
# with other enabled options.
ctk_enable_option(Widgets "Enable Qt Widget libraries" OFF
                  CTK_LIB_Widgets)

# The CTK Plugin Framework, a dynamic component system based on OSGi.
# This will enable only the framework itself.
ctk_enable_option(PluginFramework "Enable Plugin Framework" OFF
                  CTK_LIB_PluginFramework)

# The CTK Python Wrapping
ctk_enable_option(Python_Wrapping "Wrap CTK classes using Qt meta-object system into Python language" OFF
                  CTK_LIB_Scripting/Python/Core)
mark_as_superbuild(CTK_ENABLE_Python_Wrapping)

# Build examples
# Create the logical expression containing the minium set of required options
# for the CTK_BUILD_EXAMPLES option to be ON
set(_build_examples_logical_expr)
foreach(_app ${CTK_ENABLED_APPS})
  list(APPEND _build_examples_logical_expr CTK_APP_${_app} AND)
endforeach()
if(_build_examples_logical_expr)
  list(REMOVE_AT _build_examples_logical_expr -1)
endif()

ctk_enable_option_raw(CTK_BUILD_EXAMPLES "Build examples for CTK components" OFF
                  ${_build_examples_logical_expr})

#-----------------------------------------------------------------------------
# Generate target_directories list - List of directory corresponding to the different
# libraries, plugins and applications associated with the corresponding option name.
#
# The following FOREACH loops are used to:
#   1) Update either CTK_LIBS_SUBDIRS, CTK_PLUGINS_SUBDIRS or CTK_APPS_SUBDIRS variables
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


# Create list of directories corresponding to the enabled targets
set(target_directories)

# Setup testing environment before Libs are added by simulating
# the use of 'ctk_lib_option' for CTKTesting library
set(CTK_LIB_Testing TRUE)
list(APPEND CTK_LIBS Testing)
list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/Testing^^CTK_LIB_Testing")

foreach(lib ${CTK_LIBS})
  if(CTK_LIB_${lib})
    ctkMacroAddCtkLibraryOptions(${lib})
  endif()
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}^^CTK_LIB_${lib}")
endforeach()

foreach(plugin ${CTK_PLUGINS})
  if(${plugin}_SOURCE_DIR)
    list(APPEND target_directories "${${plugin}_SOURCE_DIR}^^CTK_PLUGIN_${plugin}")
  else()
    list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${plugin}^^CTK_PLUGIN_${plugin}")
  endif()
endforeach()

foreach(app ${CTK_APPS})
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Applications/${app}^^CTK_APP_${app}")
endforeach()

# Emulate the use of 'ctk_lib_option' for CTKQtTesting library
set(CTK_LIB_QtTesting ${CTK_USE_QTTESTING})
if(CTK_USE_QTTESTING)
  list(APPEND CTK_LIBS QtTesting)
  list(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/QtTesting^^CTK_LIB_QtTesting")
endif()

#message(STATUS target_directories:${target_directories})

#-----------------------------------------------------------------------------
# Compile DGraph - An application allowing to check for cycle in DAG and also obtain the
# topological order.
try_compile(RESULT_VAR ${CTK_BINARY_DIR}/Utilities/DGraph ${CTK_SOURCE_DIR}/Utilities/DGraph
              DGraph
              CMAKE_FLAGS
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

# Set optional dependencies not captured in 'target_libraries.cmake' files.
if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  set(ctkSimplePythonShell_OPTIONAL_DEPENDENCIES CTKVisualizationVTKCore VTK_LIBRARIES)
endif()

set(CTKQtTesting_OPTIONAL_DEPENDENCIES)
if(CTK_LIB_Widgets)
  list(APPEND CTKQtTesting_OPTIONAL_DEPENDENCIES CTKWidgets)
endif()
if(CTK_LIB_Visualization/VTK/Widgets)
  list(APPEND CTKQtTesting_OPTIONAL_DEPENDENCIES CTKVisualizationVTKWidgets)
endif()

#-----------------------------------------------------------------------------
# Let's make sure the enabled/disabled libraries, plugins or applications are coherent
#
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}")
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_EXTERNALS)
ctkMacroValidateBuildOptions("${CTK_BINARY_DIR}" "${DGraph_EXECUTABLE}" "${target_directories}")

#-----------------------------------------------------------------------------
# CTK Python wrapping
# Enable CTK_LIB_Scripting/Python/Core if CTK_ENABLE_Python_Wrapping is ON
#
set(CTK_WRAP_PYTHONQT_LIGHT ${CTK_ENABLE_Python_Wrapping})

set(logical_expr CTK_WRAP_PYTHONQT_LIGHT)
if((${logical_expr}) AND NOT CTK_LIB_Scripting/Python/Core)
  set(CTK_LIB_Scripting/Python/Core ON CACHE BOOL "Build the Python Core library" FORCE)
  set(enabling_msg)
  ctk_option_logical_expression_to_message(enabling_msg "${logical_expr}")
  message("Enabling [CTK_LIB_Scripting/Python/Core] because of [${enabling_msg}] evaluates to True")
endif()

set(logical_expr CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
if(${logical_expr} AND NOT CTK_LIB_Visualization/VTK/Core)
  set(CTK_LIB_Visualization/VTK/Core ON CACHE BOOL "Build the VTK Core library" FORCE)
  set(enabling_msg)
  ctk_option_logical_expression_to_message(enabling_msg "${logical_expr}")
  message("Enabling [CTK_LIB_Visualization/VTK/Core] because of [${enabling_msg}] evaluates to True")
endif()

# Check if dependencies are satisfied
if(CTK_LIB_Scripting/Python/Core)
  find_package(PythonInterp)
  if(NOT PYTHONINTERP_FOUND)
    message(FATAL_ERROR "PYTHON_EXECUTABLE variable should be set to build CTK_LIB_Scripting/Python")
  endif()
  find_package(PythonLibs)
  if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR "PYTHON_LIBRARIES and PYTHON_INCLUDE_DIRS should be set to build CTK_LIB_Scripting/Python")
  endif()
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
if(subproject_count EQUAL 0)
  set(CTEST_PROJECT_SUBPROJECTS CTKCore)
endif()

#-----------------------------------------------------------------------------
# CTK dependencies - Projects should be TOPOLOGICALLY ordered
#-----------------------------------------------------------------------------
set(CTK_DEPENDENCIES
  Log4Qt
  VTK
  PythonQt
  DCMTK
  ZMQ
  QtSOAP
  qxmlrpc
  qRestAPI
  OpenIGTLink
  XIP
  ITK
  QtTesting
  )

if(BUILD_TESTING)
  list(APPEND CTK_DEPENDENCIES CTKData)
endif()

if(CTK_USE_KWSTYLE)
  list(APPEND CTK_DEPENDENCIES KWStyle)
endif()

#-----------------------------------------------------------------------------
# Check out the ExternalProjectsContrib repository
if(CTK_USE_CONTRIBUTED_PLUGINS)
  if(CTK_SUPERBUILD)
    ctkFunctionCheckoutRepo(
      NAME ExternalProjectsContrib
      GIT_URL github.com/commontk/ExternalProjectsContrib.git
      GIT_TAG 4c944ef
      )
    mark_as_superbuild(ExternalProjectsContrib_DIR:PATH)
  endif()

  file(GLOB _contrib_scripts ${ExternalProjectsContrib_DIR}/*.cmake)
  foreach(_contrib_script ${_contrib_scripts})
    get_filename_component(_script_name ${_contrib_script} NAME_WE)
    list(APPEND CTK_DEPENDENCIES ${_script_name})
    set(${_script_name}_FILEPATH ${_contrib_script})
  endforeach()
endif()

include(CMake/ctkBlockCheckDependencies.cmake)

#-----------------------------------------------------------------------------
# Superbuild script
#

if(CTK_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

if(BUILD_TESTING)
  add_subdirectory(CMake/Testing)
endif()

#-----------------------------------------------------------------------------
# Expand variables containing include and library directories for external projects
# This relies on the variable EXTERNAL_TARGETS set in ctkMacroValidateBuildOptions
foreach(_external_target ${EXTERNAL_TARGETS})
  set(_package_name ${${_external_target}_FIND_PACKAGE_CMD})
  if(_package_name)
    superbuild_is_external_project_includable(${_package_name} required)
    if(required)
      #message("Calling find_package(${_package_name})")
      find_package(${_package_name} REQUIRED)
    endif()
  endif()
endforeach()

foreach(_external_target ${EXTERNAL_TARGETS})
  set(_package_name ${${_external_target}_FIND_PACKAGE_CMD})
  if("${_package_name}" STREQUAL "")
    set(_package_name ${_external_target})
  endif()
  if(${_external_target}_INCLUDE_DIRS)
    #message("[${_package_name}] Resolving include variables: ${${_external_target}_INCLUDE_DIRS}")
    set(_updated_include_dirs)
    foreach(_include_variable ${${_external_target}_INCLUDE_DIRS})
      set(_expanded_include_variable ${_include_variable})
      if(${_include_variable})
        #message("[${_package_name}] Expanding [${_include_variable}] into [${${_include_variable}}]")
        set(_expanded_include_variable ${${_include_variable}})
      endif()
      foreach(_include_dir ${_expanded_include_variable})
        if(EXISTS ${_include_dir})
          #message("[${_package_name}] Appending ${_include_dir}")
          list(APPEND _updated_include_dirs ${_include_dir})
        else()
          message(STATUS "[${_package_name}] Skipping nonexistent include directory or not set variable [${_include_dir}]")
        endif()
      endforeach()
      #message("[${_package_name}] New dirs: ${_updated_include_dirs}")
    endforeach()
    set(${_external_target}_INCLUDE_DIRS ${_updated_include_dirs})
    #message("[${_package_name}] Appended dirs: ${${_external_target}_INCLUDE_DIRS}")
  endif()
  if(${_external_target}_LIBRARY_DIRS)
    #message("[${_package_name}] Resolving library variables: ${${_external_target}_LIBRARY_DIRS}")
    set(_updated_library_dirs)
    foreach(_library_variable ${${_external_target}_LIBRARY_DIRS})
      set(_expanded_library_variable ${_library_variable})
      if(${_library_variable})
        #message("[${_package_name}] Expanding [${_library_variable}] into [${${_library_variable}}]")
        set(_expanded_library_variable ${${_library_variable}})
      endif()
      foreach(_library_dir ${_expanded_library_variable})
        if(EXISTS ${_library_dir})
          #message("[${_package_name}] Appending ${_library_dir}")
          list(APPEND _updated_library_dirs ${_library_dir})
        else()
          message(STATUS "[${_package_name}] Skipping nonexistent library directory or not set variable [${_library_dir}]")
        endif()
      endforeach()
      #message("[${_package_name}] New dirs: ${_updated_library_dirs}")
    endforeach()
    set(${_external_target}_LIBRARY_DIRS ${_updated_library_dirs})
    #message("[${_package_name}] Appended dirs: ${${_external_target}_LIBRARY_DIRS}")
  endif()
endforeach()

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
configure_file(${CTK_SOURCE_DIR}/CMake/UseCTK.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake COPYONLY)

install(
  FILES ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake
  DESTINATION ${CTK_INSTALL_CMAKE_DIR} COMPONENT Development
  )

set(CTK_CONFIG_H_INCLUDE_DIR ${CTK_BINARY_DIR})

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
set(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
set(CMAKE_C_FLAGS ${CTK_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT} ${CTK_EXE_LINKER_FLAGS}" CACHE STRING "Flags used when linking executables" FORCE)

#-----------------------------------------------------------------------------
# Set the header template which defines custom export/import macros
# for shared libraries
#
set(CTK_EXPORT_HEADER_TEMPLATE "${CTK_SOURCE_DIR}/Libs/ctkExport.h.in")

#-----------------------------------------------------------------------------
# Add CTK library subdirectories
#
foreach(lib ${CTK_LIBS})
  if(CTK_LIB_${lib})
    add_subdirectory(Libs/${lib})
  endif()
  # Always add the Documentation sub-directory for additional processing
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}/Documentation/CMakeLists.txt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}/Documentation)
  endif()
endforeach()

# Add the CommandLineModules root directory for additional processing
add_subdirectory(Libs/CommandLineModules)

#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
foreach(plugin ${CTK_PLUGINS})
  if(CTK_PLUGIN_${plugin})
    if(${plugin}_SOURCE_DIR)
      add_subdirectory(${${plugin}_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Plugins/${plugin})
    else()
      add_subdirectory(Plugins/${plugin})
    endif()
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
foreach(app ${CTK_APPS})
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
add_subdirectory(CMake/LastConfigureStep)

