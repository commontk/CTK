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
SET(ctk_libs
  Core
  Widgets
  DICOM/Core
  DICOM/Widgets
  Visualization/XIP
  )

#-----------------------------------------------------------------------------
# CTK Plugins
#
SET(ctk_plugins
  org.commontk.cli
  org.commontk.eventbus
  )

#-----------------------------------------------------------------------------
# CTK Applications
#
SET(ctk_applications
  ctkCLIPluginExplorer
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
FOREACH(lib ${ctk_libs})
  OPTION(CTK_LIB_${lib} "Enable ${lib} Library." ON)
  ctkMacroAddCtkLibraryOptions(${lib})
ENDFOREACH()

# Build options associated with CTK plugins
FOREACH(plugin ${ctk_plugins})
  OPTION(CTK_PLUGIN_${plugin} "Build ${plugin} Plugin." ON)
ENDFOREACH()

# Build options associated with CTK applications
FOREACH(app ${ctk_applications})
  OPTION(CTK_APP_${app} "Build ${app} Application." ON)
ENDFOREACH()

#-----------------------------------------------------------------------------
# Check dependencies
#
ctkMacroGetAllTargetLibraries("${ctk_libs}" "Libs" ALL_TARGET_LIBRARIES)
ctkMacroGetAllTargetLibraries("${ctk_plugins}" "Plugins" ALL_TARGET_LIBRARIES)
ctkMacroGetAllTargetLibraries("${ctk_applications}" "Applications" ALL_TARGET_LIBRARIES)
#MESSAGE(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

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
# Add CTK library subdirectories
#
FOREACH(lib ${ctk_libs})
  IF (CTK_LIB_${lib})
    ADD_SUBDIRECTORY(Libs/${lib})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK plugin subdirectories
#
FOREACH(plugin ${ctk_plugins})
  IF (CTK_PLUGIN_${plugin})
    ADD_SUBDIRECTORY(Plugins/${plugin})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add CTK application subdirectories
#
FOREACH(app ${ctk_applications})
  IF (CTK_APP_${app})
    ADD_SUBDIRECTORY(Applications/${app})
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
# Add general purpose subdirectories
#
ADD_SUBDIRECTORY(Testing)
ADD_SUBDIRECTORY(Examples)
    
#-----------------------------------------------------------------------------
# Style Checking configuration
#
INCLUDE(Utilities/KWStyle/KWStyle.cmake)
