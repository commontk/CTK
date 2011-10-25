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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

SET(project_policies
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
FOREACH(policy ${project_policies})
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
if(APPLE)
  # Note: By setting CMAKE_OSX_* variables before any enable_language() or project() calls,
  #       we ensure that the bitness will be properly detected.
  include(${CMAKE_SOURCE_DIR}/CMake/ctkBlockSetCMakeOSXVariables.cmake)
endif()

#-----------------------------------------------------------------------------
PROJECT(CTK)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Library mode: SHARED (default) or STATIC
#
SET(CTK_LIBRARY_MODE "SHARED")

OPTION(CTK_BUILD_SHARED_LIBS "Build CTK libraries as shared module." ON)
MARK_AS_ADVANCED(CTK_BUILD_SHARED_LIBS)
IF(NOT CTK_BUILD_SHARED_LIBS)
  SET(CTK_LIBRARY_MODE "STATIC")
ENDIF()

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
OPTION(CTK_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(CTK_SUPERBUILD)

#-----------------------------------------------------------------------------
# Output directories.
#
FOREACH(type LIBRARY RUNTIME ARCHIVE)
  # Make sure the directory exists
  IF(DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY
     AND NOT EXISTS ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    MESSAGE(FATAL_ERROR "CTK_CMAKE_${type}_OUTPUT_DIRECTORY is set to a non-existing directory [${CTK_CMAKE_${type}_OUTPUT_DIRECTORY}]")
  ENDIF()

  IF(CTK_SUPERBUILD)
    SET(output_dir ${CTK_BINARY_DIR}/bin)
    IF(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      SET(CTK_CMAKE_${type}_OUTPUT_DIRECTORY ${CTK_BINARY_DIR}/CTK-build/bin)
    ENDIF()
  ELSE()
    IF(NOT DEFINED CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
      SET(output_dir ${CTK_BINARY_DIR}/bin)
    ELSE()
      SET(output_dir ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY})
    ENDIF()
  ENDIF()
  SET(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
  IF(NOT DEFINED CTK_PLUGIN_${type}_OUTPUT_DIRECTORY)
    SET(CTK_PLUGIN_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# CTK version number.  An even minor number corresponds to releases.
#
SET(CTK_MAJOR_VERSION 0)
SET(CTK_MINOR_VERSION 1)
SET(CTK_BUILD_VERSION 0)
SET(CTK_VERSION
    "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}.${CTK_BUILD_VERSION}")

# Append the library version information to the library target
# properties.  A parent project may set its own properties and/or may
# block this.
IF(NOT CTK_NO_LIBRARY_VERSION)
  SET(CTK_LIBRARY_PROPERTIES ${CTK_LIBRARY_PROPERTIES}
    VERSION "${CTK_VERSION}"
    SOVERSION "${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}"
    )
ENDIF()

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
IF(NOT CTK_INSTALL_BIN_DIR)
  SET(CTK_INSTALL_BIN_DIR "bin")
ENDIF()
IF(NOT CTK_INSTALL_LIB_DIR)
  SET(CTK_INSTALL_LIB_DIR "lib/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
ENDIF()
IF(NOT CTK_INSTALL_INCLUDE_DIR)
  SET(CTK_INSTALL_INCLUDE_DIR "include/ctk-${CTK_MAJOR_VERSION}.${CTK_MINOR_VERSION}")
ENDIF()
IF(NOT CTK_INSTALL_DOC_DIR)
  SET(CTK_INSTALL_DOC_DIR "doc")
ENDIF()

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
SET(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_INCLUDE_DIRS, CTK_BASE_LIBRARIES and CTK_WRAPPED_LIBRARIES_PYTHONQT
#
SET(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK base libraries" FORCE)
SET(CTK_BASE_INCLUDE_DIRS CACHE INTERNAL "CTK includes" FORCE)
SET(CTK_WRAPPED_LIBRARIES_PYTHONQT CACHE INTERNAL "CTK libraries wrapped using PythonQt" FORCE)

# Variable use in CTKConfig.cmake.in
SET(CTK_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)
SET(CTK_PLUGIN_LIBRARIES CACHE INTERNAL "CTK plugins" FORCE)

# Used by CTKGenerateCTKConfig.cmake and also used to reference script from other scripts
SET(CTK_CMAKE_DIR ${CTK_SOURCE_DIR}/CMake)
SET(CTK_CMAKE_UTILITIES_DIR ${CTK_SOURCE_DIR}/Utilities/CMake)

#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#
INCLUDE(CMake/ctkMacroParseArguments.cmake)
INCLUDE(CMake/ctkMacroSetPaths.cmake)
INCLUDE(CMake/ctkMacroListFilter.cmake)
INCLUDE(CMake/ctkMacroBuildLib.cmake)
INCLUDE(CMake/ctkMacroBuildLibWrapper.cmake)
INCLUDE(CMake/ctkMacroBuildPlugin.cmake)
INCLUDE(CMake/ctkMacroBuildApp.cmake)
INCLUDE(CMake/ctkMacroCompilePythonScript.cmake)
INCLUDE(CMake/ctkMacroWrapPythonQt.cmake)
INCLUDE(CMake/ctkMacroSetupQt.cmake)
INCLUDE(CMake/ctkMacroTargetLibraries.cmake) # Import multiple macros
INCLUDE(CMake/ctkFunctionExtractOptionNameAndValue.cmake)
INCLUDE(CMake/ctkMacroValidateBuildOptions.cmake)
INCLUDE(CMake/ctkMacroAddCtkLibraryOptions.cmake)
INCLUDE(CMake/ctkFunctionGenerateDGraphInput.cmake)
INCLUDE(CMake/ctkFunctionGenerateProjectXml.cmake)
INCLUDE(CMake/ctkFunctionGeneratePluginManifest.cmake)
INCLUDE(CMake/ctkMacroGeneratePluginResourceFile.cmake)
INCLUDE(CMake/ctkFunctionCheckCompilerFlags.cmake)
INCLUDE(CMake/ctkFunctionGetIncludeDirs.cmake)
INCLUDE(CMake/ctkFunctionGetLibraryDirs.cmake)
INCLUDE(CMake/ctkFunctionGetGccVersion.cmake)

#-----------------------------------------------------------------------------
# Qt Designer Plugins

OPTION(BUILD_QTDESIGNER_PLUGINS "Build Qt Designer plugins" ON)
IF(BUILD_QTDESIGNER_PLUGINS)
  INCLUDE(CMake/ctkMacroBuildQtDesignerPlugin.cmake)
ENDIF()

#-----------------------------------------------------------------------------
# Testing
#
OPTION(BUILD_TESTING "Test the project" ON)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE(CTest)
  SET(CPP_TEST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)

  # Setup file for setting custom ctest vars
  CONFIGURE_FILE(
    CMake/CTestCustom.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
    @ONLY
    )

  # Configuration for the CMake-generated test driver
  SET(CMAKE_TESTDRIVER_EXTRA_INCLUDES "#include <stdexcept>")
  SET(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "
    try
      {")
  SET(CMAKE_TESTDRIVER_AFTER_TESTMAIN "    }
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
ENDIF()

#-----------------------------------------------------------------------------
# Coverage
#
OPTION(WITH_COVERAGE "Enable/Disable coverage" OFF)

#-----------------------------------------------------------------------------
# Documentation
#
OPTION(DOCUMENTATION_TARGET_IN_ALL "Include the custom target for building documentation in 'all'" OFF)
MARK_AS_ADVANCED(DOCUMENTATION_TARGET_IN_ALL)

SET(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
 CACHE PATH "Where documentation archives should be stored")
MARK_AS_ADVANCED(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)

#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
SET(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
MARK_AS_ADVANCED(ADDITIONAL_C_FLAGS)
SET(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
MARK_AS_ADVANCED(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set symbol visibility Flags
#

ctkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)

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
IF(CMAKE_COMPILER_IS_GNUCXX AND NOT ${GCC_VERSION} VERSION_LESS "4.5" AND NOT MINGW)
  SET(VISIBILITY_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
ENDIF()

#-----------------------------------------------------------------------------
# Set coverage Flags
#
IF(WITH_COVERAGE)
  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(coverage_flags "-g -fprofile-arcs -ftest-coverage -O0 -DNDEBUG")
    SET(COVERAGE_CXX_FLAGS ${coverage_flags})
    SET(COVERAGE_C_FLAGS ${coverage_flags})
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# CTK C/CXX Flags
#
SET(CTK_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
SET(CTK_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings -D_FORTIFY_SOURCE=2")

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-Wl,--no-undefined" cflags)

  # With older version of gcc supporting the flag -fstack-protector-all, an extra dependency to libssp.so
  # is introduced. If gcc is smaller than 4.4.0 and the build type is Release let's not include the flag.
  # Doing so should allow to build package made for distribution using older linux distro.
  IF(${GCC_VERSION} VERSION_GREATER "4.4.0" OR (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ${GCC_VERSION} VERSION_LESS "4.4.0"))
    ctkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)
  ENDIF()
  IF(MINGW)
    # suppress warnings about auto imported symbols
    SET(CTK_CXX_FLAGS "-Wl,--enable-auto-import ${CTK_CXX_FLAGS}")
  ENDIF()

  SET(CTK_C_FLAGS "${cflags} ${CTK_C_FLAGS}")
  SET(CTK_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${CTK_CXX_FLAGS}")
ENDIF()

IF(MSVC)
  SET(msvc_suppressed_warnings
    "/wd4290" # C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
  )
  SET(CTK_CXX_FLAGS "${CTK_CXX_FLAGS} ${msvc_suppressed_warnings}")
ENDIF()

#-----------------------------------------------------------------------------
# QT
#
ctkMacroSetupQt()

# Update CTK_BASE_LIBRARIES with QT libraries
IF(QT4_FOUND)
  SET(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK base libraries" FORCE)
ENDIF()

#-----------------------------------------------------------------------------
# CTK Libraries - Use ON or OFF to indicate if the library should be built by default
#
SET(CTK_LIBS
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
SET(CTK_PLUGINS
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
  org.commontk.qtmobility.service:OFF
  org.commontk.slicermodule:OFF
  )

#-----------------------------------------------------------------------------
# CTK Applications - Use ON or OFF to indicate if the application should be built by default
#
SET(CTK_APPLICATIONS
  ctkCLIPluginExplorer:OFF
  ctkDICOM:OFF
  ctkDICOMIndexer:OFF
  ctkDICOMDemoSCU:OFF
  ctkDICOMQuery:OFF
  ctkDICOMRetrieve:OFF
  ctkDICOMQueryRetrieve:OFF
  ctkExampleHost:OFF
  ctkExampleHostedApp:OFF
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
MARK_AS_ADVANCED(CMAKE_INSTALL_PREFIX)
MARK_AS_ADVANCED(DART_TESTING_TIMEOUT)

# KWStyle
OPTION(CTK_USE_KWSTYLE     "Enable sourcecode-based style tests." OFF)
#MARK_AS_ADVANCED(CTK_USE_KWSTYLE)

#---------------------------------------------------------------------------
# Will contain a list of sub-directory without option ON or OFF
#
SET(CTK_LIBS_SUBDIRS )
SET(CTK_PLUGINS_SUBDIRS )
SET(CTK_APPLICATIONS_SUBDIRS )

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
#     SET(ctk_library_options
#       OPT1:OFF
#       OPT2:ON
#       )

# Build options associated with CTK libraries
SET(ctk_lib_options_list) # This list will be updated in ctkFunctionExtractOptionNameAndValue
FOREACH(lib ${CTK_LIBS})
  ctkFunctionExtractOptionNameAndValue(${lib} lib_name lib_value)
  OPTION(CTK_LIB_${lib_name} "Enable ${lib_name} Library." ${lib_value})
  IF(CTK_LIB_${lib_name})
    ctkMacroAddCtkLibraryOptions(${lib_name})
  ENDIF()
  LIST(APPEND CTK_LIBS_SUBDIRS "${lib_name}")
ENDFOREACH()

# Build options associated with CTK plugins
FOREACH(plugin ${CTK_PLUGINS})
  ctkFunctionExtractOptionNameAndValue(${plugin} plugin_name plugin_value)
  OPTION(CTK_PLUGIN_${plugin_name} "Build ${plugin_name} Plugin." ${plugin_value})
  LIST(APPEND CTK_PLUGINS_SUBDIRS "${plugin_name}")
ENDFOREACH()

# Build options associated with CTK applications
FOREACH(app ${CTK_APPLICATIONS})
  ctkFunctionExtractOptionNameAndValue(${app} app_name app_value)
  OPTION(CTK_APP_${app_name} "Build ${app_name} Application." ${app_value})
  LIST(APPEND CTK_APPLICATIONS_SUBDIRS "${app_name}")
ENDFOREACH()

#-----------------------------------------------------------------------------
# Generate target_directories list - List of directory corresponding to the different
# libraries, plugins and applications associated with the corresponding option name.
#

# Create list of directories corresponding to the enabled targets
SET(target_directories)

FOREACH(lib ${CTK_LIBS_SUBDIRS})
  SET(option_name CTK_LIB_${lib})
  LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}^^${option_name}")
ENDFOREACH()

FOREACH(plugin ${CTK_PLUGINS_SUBDIRS})
  SET(option_name CTK_PLUGIN_${plugin})
  LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${plugin}^^${option_name}")
ENDFOREACH()

FOREACH(app ${CTK_APPLICATIONS_SUBDIRS})
  SET(option_name CTK_APP_${app})
  LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Applications/${app}^^${option_name}")
ENDFOREACH()

#MESSAGE(STATUS target_directories:${target_directories})

#-----------------------------------------------------------------------------
# Compile DGraph - An application allowing to check for cycle in DAG and also obtain the
# topological order.
TRY_COMPILE(RESULT_VAR ${CTK_BINARY_DIR}/Utilities/DGraph ${CTK_SOURCE_DIR}/Utilities/DGraph
              DGraph
              CMAKE_FLAGS
                -DQT_QMAKE_EXECUTABLE:FILE=${QT_QMAKE_EXECUTABLE}
                -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE
              OUTPUT_VARIABLE output)
IF(NOT RESULT_VAR)
  MESSAGE(FATAL_ERROR "Failed to compile DGraph application.\n${output}")
ENDIF()
FIND_PROGRAM(DGraph_EXECUTABLE DGraph
  "${CTK_BINARY_DIR}/Utilities/DGraph/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/bin/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Debug/"
  "${CTK_BINARY_DIR}/Utilities/DGraph/Release/")
MARK_AS_ADVANCED(DGraph_EXECUTABLE)

#-----------------------------------------------------------------------------
# Let's make sure the enabled/disabled libraries, plugins or applications are coherent
#
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}")
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_EXTERNALS)
ctkMacroValidateBuildOptions("${CTK_BINARY_DIR}" "${DGraph_EXECUTABLE}" "${target_directories}")

#-----------------------------------------------------------------------------
# CTK Python wrapping - Propose the option only if CTK_LIB_Scriping/Python/Core is ON
IF(CTK_LIB_Scripting/Python/Core)
  find_package(PythonInterp)
  IF(NOT PYTHONINTERP_FOUND)
    MESSAGE(SEND_ERROR "PYTHON_EXECUTABLE variable should be set to build CTK_LIB_Scripting/Python")
  ENDIF()
  FIND_PACKAGE(PythonLibs)
  IF(NOT PYTHONLIBS_FOUND)
    MESSAGE(FATAL_ERROR "PYTHON_LIBRARIES and PYTHON_INCLUDE_DIRS should be set to build CTK_LIB_Scripting/Python")
  ENDIF()
  OPTION(CTK_WRAP_PYTHONQT_FULL "Experimental - Wrap CTK classes using Qt meta-object system into Python language" OFF)
  MARK_AS_ADVANCED(CTK_WRAP_PYTHONQT_FULL)
  OPTION(CTK_WRAP_PYTHONQT_LIGHT "Wrap CTK classes using Qt meta-object system into Python language" OFF)
  IF(CTK_WRAP_PYTHONQT_FULL AND CTK_WRAP_PYTHONQT_LIGHT)
    MESSAGE(SEND_ERROR "CTK_WRAP_PYTHONQT_{LIGHT,FULL} options are mutually exclusive. Please enable only one !")
  ENDIF()
ELSE()
  # Note that an unset boolean variable is considered to be False
  UNSET(CTK_WRAP_PYTHONQT_FULL CACHE)
  UNSET(CTK_WRAP_PYTHONQT_LIGHT CACHE)
ENDIF()

#-----------------------------------------------------------------------------
# DGraph
#

# Generate DGraph input file expected by DGraph
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" WITH_OPTION)

# Obtain list of target ordered topologically
ctkMacroSetPaths("${QT_INSTALLED_LIBRARY_DIR}")
EXECUTE_PROCESS(
  COMMAND "${DGraph_EXECUTABLE}" "${CTK_BINARY_DIR}/DGraphInput.txt"
  WORKING_DIRECTORY ${CTK_BINARY_DIR}
  RESULT_VARIABLE RESULT_VAR
  OUTPUT_VARIABLE CTEST_PROJECT_SUBPROJECTS_OUTPUT
  ERROR_VARIABLE error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
IF(RESULT_VAR)
  MESSAGE(FATAL_ERROR "Failed to obtain list of target ordered topologically.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
ENDIF()

# Convert 'CTEST_PROJECT_SUBPROJECTS_OUTPUT' to a list
STRING(REPLACE " " "\\;" CTEST_PROJECT_SUBPROJECTS "${CTEST_PROJECT_SUBPROJECTS_OUTPUT}")
SET(CTEST_PROJECT_SUBPROJECTS ${CTEST_PROJECT_SUBPROJECTS})

# If the list of subproject is empty, let's at least build CTKCore
LIST(LENGTH CTEST_PROJECT_SUBPROJECTS subproject_count)
IF (subproject_count EQUAL 0)
  SET(CTEST_PROJECT_SUBPROJECTS CTKCore)
ENDIF()

# Configure CTestConfigSubProject.cmake used that could be used by CTest scripts
CONFIGURE_FILE(${CTK_SOURCE_DIR}/CTestConfigSubProject.cmake.in
               ${CTK_BINARY_DIR}/CTestConfigSubProject.cmake)

#-----------------------------------------------------------------------------
# Project.xml
#

# Generate Project.xml file expected by the CTest driver script
ctkFunctionGenerateProjectXml(${CTK_BINARY_DIR} ${PROJECT_NAME} "${target_directories}" ${CTK_SUPERBUILD})

#-----------------------------------------------------------------------------
# Superbuild script
#

IF(CTK_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
ENDIF()


#=============================================================================
# End of Superbuild
#=============================================================================


#-----------------------------------------------------------------------------
# Expand variables containing include and library directories for external projects
# This relies on the variable EXTERNAL_TARGETS set in ctkMacroValidateBuildOptions
FOREACH(_external_target ${EXTERNAL_TARGETS})
  IF(${_external_target}_FIND_PACKAGE_CMD)
    #MESSAGE("Calling FIND_PACKAGE(${${_external_target}_FIND_PACKAGE_CMD})")
    FIND_PACKAGE(${${_external_target}_FIND_PACKAGE_CMD} REQUIRED)
  ENDIF()
ENDFOREACH()

FOREACH(_external_target ${EXTERNAL_TARGETS})
  IF(${_external_target}_INCLUDE_DIRS)
    STRING(REPLACE "^" ";" _include_variable_list "${${_external_target}_INCLUDE_DIRS}")
    IF(_include_variable_list)
      #MESSAGE("[${_external_target}] Resolving include variables: ${${_external_target}_INCLUDE_DIRS}")
      SET(${_external_target}_INCLUDE_DIRS "")
      FOREACH(_include_variable ${_include_variable_list})
        #MESSAGE("[${_external_target}] Appending ${${_include_variable}}")
        IF(${_include_variable})
          LIST(APPEND ${_external_target}_INCLUDE_DIRS ${${_include_variable}})
        ELSE()
          LIST(APPEND ${_external_target}_INCLUDE_DIRS ${_include_variable})
        ENDIF()
        #MESSAGE("[${_external_target}] New dirs: ${${_external_target}_INCLUDE_DIRS}")
      ENDFOREACH()
      #MESSAGE("[${_external_target}] Appended dirs: ${${_external_target}_INCLUDE_DIRS}")
    ENDIF()
  ENDIF()
  IF(${_external_target}_LIBRARY_DIRS)
    STRING(REPLACE "^" ";" _library_variable_list "${${_external_target}_LIBRARY_DIRS}")
    IF(_library_variable_list)
      #MESSAGE("[${_external_target}] Resolving library variables: ${${_external_target}_LIBRARY_DIRS}")
      SET(${_external_target}_LIBRARY_DIRS "")
      FOREACH(_library_variable ${_library_variable_list})
        #MESSAGE("[${_external_target}] Appending ${${_library_variable}}")
        IF(${_library_variable})
          LIST(APPEND ${_external_target}_LIBRARY_DIRS ${${_library_variable}})
        ELSE()
          LIST(APPEND ${_external_target}_LIBRARY_DIRS ${_library_variable})
        ENDIF()
        #MESSAGE("[${_external_target}] New dirs: ${${_external_target}_LIBRARY_DIRS}")
      ENDFOREACH()
      #MESSAGE("[${_external_target}] Appended dirs: ${${_external_target}_LIBRARY_DIRS}")
    ENDIF()
  ENDIF()
ENDFOREACH()

# Since PYTHONQT_USE_VTK library option can be enabled independently of
# Visualization/VTK/Core, let's make sure VTK has been properly discovered
IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  FIND_PACKAGE(VTK REQUIRED)
ENDIF()
SET(CTK_WRAP_PYTHONQT_USE_VTK ${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK})

#-----------------------------------------------------------------------------
# CTK_SUPERBUILD_BINARY_DIR

# If CTK_SUPERBUILD_BINARY_DIR isn't defined, it means CTK is *NOT* build using Superbuild.
# In that specific case, CTK_SUPERBUILD_BINARY_DIR should default to CTK_BINARY_DIR
IF(NOT DEFINED CTK_SUPERBUILD_BINARY_DIR)
  SET(CTK_SUPERBUILD_BINARY_DIR ${CTK_BINARY_DIR})
ENDIF()

#-----------------------------------------------------------------------------
# Configure files with settings
#
CONFIGURE_FILE(${CTK_SOURCE_DIR}/UseCTK.cmake.in
               ${CTK_SUPERBUILD_BINARY_DIR}/UseCTK.cmake COPYONLY IMMEDIATE)

SET(CTK_CONFIG_H_INCLUDE_DIR ${CTK_BINARY_DIR})

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
SET(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
SET(CMAKE_C_FLAGS ${CTK_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

#-----------------------------------------------------------------------------
# Set the header template which defines custom export/import macros
# for shared libraries
#
SET(CTK_EXPORT_HEADER_TEMPLATE "${CTK_SOURCE_DIR}/Libs/ctkExport.h.in")

#-----------------------------------------------------------------------------
# Add CTK library subdirectories
#
FOREACH(lib ${CTK_LIBS_SUBDIRS})
  IF (CTK_LIB_${lib})
    ADD_SUBDIRECTORY(Libs/${lib})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
FOREACH(plugin ${CTK_PLUGINS_SUBDIRS})
  IF(CTK_PLUGIN_${plugin})
    ADD_SUBDIRECTORY(Plugins/${plugin})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
FOREACH(app ${CTK_APPLICATIONS_SUBDIRS})
  IF (CTK_APP_${app})
    ADD_SUBDIRECTORY(Applications/${app})
  ENDIF()
ENDFOREACH()

IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Applications/Testing)
ENDIF(BUILD_TESTING)


#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
#ADD_SUBDIRECTORY(Testing)
#ADD_SUBDIRECTORY(Examples)

#-----------------------------------------------------------------------------
# Style Checking configuration
#
INCLUDE(Utilities/KWStyle/KWStyle.cmake)

#---------------------------------------------------------------------------
# Documentation
#
ADD_SUBDIRECTORY( Documentation )

#-----------------------------------------------------------------------------
# The commands in this directory are intended to be executed as
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
ADD_SUBDIRECTORY(Utilities/LastConfigureStep)

