CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FOREACH(policy CMP0003)
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF()
ENDFOREACH()

PROJECT(CTK)

# Default to shared library
SET(CTK_LIBRARY_MODE "SHARED")
SET(CTK_BUILD_SHARED_LIBS TRUE)

#-----------------------------------------------------------------------------
# Output directories.
#
IF(NOT LIBRARY_OUTPUT_PATH)
  SET(LIBRARY_OUTPUT_PATH ${CTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${CTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
SET(CTK_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
SET(CTK_EXECUTABLE_PATH ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})

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
SET(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)
SET(CTK_BASE_INCLUDE_DIRS CACHE INTERNAL "CTK includes" FORCE)

#-----------------------------------------------------------------------------
# CMake Macro(s)
# 
INCLUDE(CMake/ctkMacroParseArguments.cmake)
INCLUDE(CMake/ctkMacroListFilter.cmake)
INCLUDE(CMake/ctkMacroBuildQtLib.cmake)
INCLUDE(CMake/ctkMacroBuildQtPlugin.cmake)
INCLUDE(CMake/ctkMacroBuildQtApp.cmake)
INCLUDE(CMake/ctkMacroSetupQt.cmake)
INCLUDE(CMake/ctkMacroTargetLibraries.cmake) # Import multiple macros
INCLUDE(CMake/ctkMacroAddCtkLibraryOptions.cmake)
INCLUDE(CMake/ctkMacroGenerateDGraphInput.cmake)
INCLUDE(CMake/ctkMacroGenerateProjectXml.cmake)

#-----------------------------------------------------------------------------
# Patch program
#
FIND_PROGRAM(CTK_PATCH_EXECUTABLE patch
  "C:/Program Files/GnuWin32/bin"
  "C:/Program Files (x86)/GnuWin32/bin")
MARK_AS_ADVANCED(CTK_PATCH_EXECUTABLE)

#-----------------------------------------------------------------------------
# Testing
#
OPTION(BUILD_TESTING "Test the project" ON)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE(CTest)
  SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
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
  SET(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK libraries" FORCE)
ENDIF()

#-----------------------------------------------------------------------------
# CTK Libraries
#
SET(CTK_LIBS
  Core
  Widgets
  DICOM/Core
  DICOM/Widgets
  Visualization/XIP
  )

#-----------------------------------------------------------------------------
# CTK Plugins
#
SET(CTK_PLUGINS
  #org.commontk.cli
  )

#-----------------------------------------------------------------------------
# CTK Applications
#
SET(CTK_APPLICATIONS
  ctkDICOM
  ctkDICOMIndexer
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

# Build options associated with CTK libraries
# Note also that if
# the file Libs/<DIR>/<LIBNAME>/ctk_library_options.cmake exists and look like:
#
#     SET(ctk_library_options
#       OPT1:OFF
#       OPT2:ON
#       )
#
# In addition to 'CTK_LIB_Visualization/XIP' option, the following ones
# will also be available in CMake configure menu:
#  CTK_LIB_<DIR>/<LIBNAME>_OPT1  (set to OFF)
#  CTK_LIB_<DIR>/<LIBNAME>_OPT2  (set to ON)
#
FOREACH(lib ${CTK_LIBS})
  OPTION(CTK_LIB_${lib} "Enable ${lib} Library." ON)
  ctkMacroAddCtkLibraryOptions(${lib})
ENDFOREACH()

# Build options associated with CTK plugins
FOREACH(plugin ${CTK_PLUGINS})
  OPTION(CTK_PLUGIN_${plugin} "Build ${plugin} Plugin." ON)
ENDFOREACH()

# Build options associated with CTK applications
FOREACH(app ${CTK_APPLICATIONS})
  OPTION(CTK_APP_${app} "Build ${app} Application." ON)
ENDFOREACH()


#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
OPTION(CTK_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(CTK_SUPERBUILD)

#-----------------------------------------------------------------------------
# Project.xml
#

# Create list of directories corresponding to the enabled targets
SET(target_directories)

FOREACH(lib ${CTK_LIBS})
  SET(option_name CTK_LIB_${lib})
  IF(${${option_name}})
    LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}^^${option_name}")
  ENDIF()
ENDFOREACH()

FOREACH(plugin ${CTK_PLUGINS})
  SET(option_name CTK_PLUGIN_${plugin})
  IF(${${option_name}})
    LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${plugin}^^${option_name}")
  ENDIF()
ENDFOREACH()

FOREACH(app ${CTK_APPLICATIONS})
  SET(option_name CTK_APP_${app})
  IF(${${option_name}})
    LIST(APPEND target_directories "${CMAKE_CURRENT_SOURCE_DIR}/Applications/${app}^^${option_name}")
  ENDIF()
ENDFOREACH()

#MESSAGE(STATUS target_directories:${target_directories})

# Generate Project.xml file expected by the CTest driver script
ctkMacroGenerateProjectXml(${CTK_BINARY_DIR} ${PROJECT_NAME} "${target_directories}" ${CTK_SUPERBUILD})

#-----------------------------------------------------------------------------
# DGraph
#

# Generate DGraph input file expected by DGraph
ctkMacroGenerateDGraphInput(${CTK_BINARY_DIR} ${PROJECT_NAME} "${target_directories}")

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

# Obtain list of target ordered topologically
EXECUTE_PROCESS(
  COMMAND ${DGraph_EXECUTABLE} ${CTK_BINARY_DIR}/DGraphInput.txt  
  WORKING_DIRECTORY ${CTK_BINARY_DIR}
  RESULT_VARIABLE RESULT_VAR
  OUTPUT_VARIABLE CTEST_PROJECT_SUBPROJECTS
  ERROR_VARIABLE error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
IF(RESULT_VAR)
  MESSAGE(FATAL_ERROR "Failed to obtain list of target ordered topologically.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
ENDIF()         

# Configure CTestConfigSubProject.cmake used that could be used by CTest scripts
CONFIGURE_FILE(${CTK_SOURCE_DIR}/CTestConfigSubProject.cmake.in
               ${CTK_BINARY_DIR}/CTestConfigSubProject.cmake)

#-----------------------------------------------------------------------------
# Superbuild script
#

IF(CTK_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
ENDIF()

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
SET(CMAKE_CXX_FLAGS ${CTK_CXX_FLAGS})
SET(CMAKE_C_FLAGS ${CTK_C_FLAGS})

#-----------------------------------------------------------------------------
# Add CTK library subdirectories
#
FOREACH(lib ${CTK_LIBS})
  IF (CTK_LIB_${lib})
    ADD_SUBDIRECTORY(Libs/${lib})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
FOREACH(plugin ${CTK_PLUGINS})
  IF (CTK_PLUGIN_${plugin})
    ADD_SUBDIRECTORY(Plugins/${plugin})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
FOREACH(app ${CTK_APPLICATIONS})
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
