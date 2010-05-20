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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

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
PROJECT(CTK)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Default to shared library
SET(CTK_LIBRARY_MODE "SHARED")
SET(CTK_BUILD_SHARED_LIBS TRUE)

#-----------------------------------------------------------------------------
# Output directories.
#
FOREACH(type LIBRARY RUNTIME ARCHIVE)
  IF(NOT CTK_CMAKE_${type}_OUTPUT_DIRECTORY)
    SET(CMAKE_${type}_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
  ELSE()
    SET(CMAKE_${type}_OUTPUT_DIRECTORY ${CTK_CMAKE_${type}_OUTPUT_DIRECTORY} CACHE INTERNAL "Single output directory for building all libraries.")
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
SET(CTK_INSTALL_BIN_DIR "bin")
SET(CTK_INSTALL_LIB_DIR "lib")
SET(CTK_INSTALL_INCLUDE_DIR "include")
SET(CTK_INSTALL_DOC_DIR "doc")

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
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
SET(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_INCLUDE_DIRS and CTK_BASE_LIBRARIES
#
SET(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK base libraries" FORCE)
SET(CTK_BASE_INCLUDE_DIRS CACHE INTERNAL "CTK includes" FORCE)

# Variable use in CTKConfig.cmake.in
SET(CTK_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)

#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#
INCLUDE(CMake/ctkMacroParseArguments.cmake)
INCLUDE(CMake/ctkMacroListFilter.cmake)
INCLUDE(CMake/ctkMacroBuildLib.cmake)
INCLUDE(CMake/ctkMacroBuildPlugin.cmake)
INCLUDE(CMake/ctkMacroBuildApp.cmake)
INCLUDE(CMake/ctkMacroBuildQtDesignerPlugin.cmake)
INCLUDE(CMake/ctkMacroSetupQt.cmake)
INCLUDE(CMake/ctkMacroTargetLibraries.cmake) # Import multiple macros
INCLUDE(CMake/ctkFunctionExtractOptionNameAndValue.cmake)
INCLUDE(CMake/ctkFunctionExecuteProcess.cmake)
INCLUDE(CMake/ctkFunctionLFtoCRLF.cmake)
INCLUDE(CMake/ctkMacroValidateBuildOptions.cmake)
INCLUDE(CMake/ctkMacroAddCtkLibraryOptions.cmake)
INCLUDE(CMake/ctkFunctionGenerateDGraphInput.cmake)
INCLUDE(CMake/ctkFunctionGenerateProjectXml.cmake)
INCLUDE(CMake/ctkMacroGeneratePluginManifest.cmake)
INCLUDE(CMake/ctkMacroGeneratePluginResourceFile.cmake)

# Used by CTKGenerateCTKConfig.cmake
SET(CTK_CMAKE_DIR ${CTK_SOURCE_DIR}/CMake)
SET(CTK_CMAKE_UTILITIES_DIR ${CTK_SOURCE_DIR}/Utilities/CMake)

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
IF(CMAKE_CXX_COMPILER_ID)
  # Set the default symbol visibility to hidden for gcc
  IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    SET(VISIBILITY_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Set coverage Flags
#
IF(WITH_COVERAGE)
  IF(CMAKE_CXX_COMPILER_ID AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    SET(coverage_flags "-g -fdiagnostics-show-option -fprofile-arcs -ftest-coverage -O3 -DNDEBUG")
    SET(COVERAGE_CXX_FLAGS ${coverage_flags})
    SET(COVERAGE_C_FLAGS ${coverage_flags})
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# CTK C/CXX Flags
#
SET(CTK_C_FLAGS "${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
SET(CTK_CXX_FLAGS "${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

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
  Messaging/Core:OFF
  Scripting/Python/Core:OFF
  Scripting/Python/Widgets:OFF
  Visualization/VTK/Core:OFF
  Visualization/VTK/Widgets:OFF
  Visualization/XIP:OFF
  )

#-----------------------------------------------------------------------------
# CTK Plugins - Use ON or OFF to indicate if the plugin should be built by default
#
SET(CTK_PLUGINS
  org.commontk.eventbus:ON
  org.commontk.cli:OFF
  )

#-----------------------------------------------------------------------------
# CTK Applications - Use ON or OFF to indicate if the application should be built by default
#
SET(CTK_APPLICATIONS
  ctkCLIPluginExplorer:OFF
  ctkPluginBrowser:OFF
  ctkDICOM:OFF
  ctkDICOMIndexer:OFF
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
# Documentation
ADD_SUBDIRECTORY( Documentation )

#---------------------------------------------------------------------------
# Will contain a list of sub-directory without option ON or OFF
#
SET(CTK_LIBS_SUBDIRS )
SET(CTK_PLUGINS_SUBDIRS )
SET(CTK_APPLICATIONS_SUBDIRS )

#-----------------------------------------------------------------------------
# Build options associated with CTK libraries
# Note also that if
# the file Libs/<DIR>/<LIBNAME>/ctk_library_options.cmake exists and look like:
#
#     SET(ctk_library_options
#       OPT1:OFF
#       OPT2:ON
#       )
#
# In addition to 'CTK_LIB_<DIR>/<LIBNAME>' option, the following ones
# will also be available in CMake configure menu:
#  CTK_LIB_<DIR>/<LIBNAME>_OPT1  (set to OFF)
#  CTK_LIB_<DIR>/<LIBNAME>_OPT2  (set to ON)
#
FOREACH(lib ${CTK_LIBS})
  ctkFunctionExtractOptionNameAndValue(${lib} lib_name lib_value)
  OPTION(CTK_LIB_${lib_name} "Enable ${lib_name} Library." ${lib_value})
  ctkMacroAddCtkLibraryOptions(${lib_name})
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
# Superbuild Option - Enabled by default
#
OPTION(CTK_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(CTK_SUPERBUILD)

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
                -DQT_QMAKE_EXECUTABLE:BOOL=${QT_QMAKE_EXECUTABLE}
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
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" FALSE)
ctkMacroValidateBuildOptions("${CTK_BINARY_DIR}" "${DGraph_EXECUTABLE}" "${target_directories}")

#-----------------------------------------------------------------------------
# DGraph
#

# Generate DGraph input file expected by DGraph
ctkFunctionGenerateDGraphInput(${CTK_BINARY_DIR} "${target_directories}" TRUE)

# Obtain list of target ordered topologically
ctkFunctionExecuteProcess(
  COMMAND "${DGraph_EXECUTABLE}" "${CTK_BINARY_DIR}/DGraphInput.txt"
  PATH_LIST \"${QT_INSTALLED_LIBRARY_DIR}\"
  WORKING_DIRECTORY ${CTK_BINARY_DIR}
  RESULT_VARIABLE RESULT_VAR
  OUTPUT_VARIABLE CTEST_PROJECT_SUBPROJECTS
  ERROR_VARIABLE error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
IF(RESULT_VAR)
  MESSAGE(FATAL_ERROR "Failed to obtain list of target ordered topologically.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
ENDIF()

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
               
#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
SET(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS})
SET(CMAKE_C_FLAGS ${CTK_C_FLAGS})

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
  IF (CTK_PLUGIN_${plugin})
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

#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
#ADD_SUBDIRECTORY(Testing)
#ADD_SUBDIRECTORY(Examples)

#-----------------------------------------------------------------------------
# Style Checking configuration
#
INCLUDE(Utilities/KWStyle/KWStyle.cmake)

#-----------------------------------------------------------------------------
# The commands in this directory are intended to be executed as
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
ADD_SUBDIRECTORY(Utilities/LastConfigureStep)

