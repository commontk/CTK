CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

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
# Update CMake module path
# 
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# Clear CTK_BASE_INCLUDE_DIRS and CTK_BASE_LIBRARIES
#
SET(CTK_BASE_LIBRARIES CACHE INTERNAL "CTK libraries" FORCE)
SET(CTK_BASE_INCLUDE_DIRS CACHE INTERNAL "CTK includes" FORCE)

#-----------------------------------------------------------------------------
# CMake Macro(s)
# 
INCLUDE(CMake/ctkMacroParseArguments.cmake)
INCLUDE(CMake/ctkMacroBuildQtLib.cmake)
INCLUDE(CMake/ctkMacroBuildQtPlugin.cmake)
INCLUDE(CMake/ctkMacroSetupQt.cmake)
INCLUDE(CMake/ctkMacroGetTargetLibraries.cmake)

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
# QT
#
ctkMacroSetupQt()

#-----------------------------------------------------------------------------
# CTK Libraries
#
SET(ctk_libs
  Core
  Widgets
  DICOM/Core
  DICOM/Widgets
  DICOM/Applications)

#-----------------------------------------------------------------------------
# CTK Plugins
#
SET(ctk_plugins
  org.commontk.cli
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
FOREACH(lib ${ctk_libs})
  OPTION(CTK_ENABLE_${lib} "Enable ${lib} Support." ON)
ENDFOREACH()

# Build options associated with CTK plugins
FOREACH(plugin ${ctk_plugins})
  OPTION(CTK_ENABLE_${plugin} "Enable ${plugin} Support." ON)
ENDFOREACH()

#-----------------------------------------------------------------------------
# Superbuild is used by default
#
OPTION(CTK_SUPERBUILD "Build CTK and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(CTK_SUPERBUILD)

IF(CTK_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
ENDIF()

#-----------------------------------------------------------------------------
# Add CTK libraries subdirectories
#
FOREACH(lib ${ctk_libs})
  IF (CTK_ENABLE_${lib})
    ADD_SUBDIRECTORY(Libs/${lib})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK plugins subdirectories
#
FOREACH(plugin ${ctk_plugins})
  IF (CTK_ENABLE_${plugin})
    ADD_SUBDIRECTORY(Plugins/${plugin})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
ADD_SUBDIRECTORY(Applications)
ADD_SUBDIRECTORY(Testing)
ADD_SUBDIRECTORY(Examples)
    
#-----------------------------------------------------------------------------
# Style Checking configuration
#
INCLUDE(Utilities/KWStyle/KWStyle.cmake)
