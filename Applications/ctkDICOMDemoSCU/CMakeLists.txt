PROJECT(ctkDICOMDemoSCU)

#
# 3rd party dependencies
#

FIND_PACKAGE(DCMTK)
IF(NOT DCMTK_FOUND)
  MESSAGE(FATAL_ERROR "error: DCMTK package is required to build ${PROJECT_NAME}")
ENDIF()

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
  
# Source files
SET(KIT_SRCS
  ctkDICOMDemoSCUMain.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildApp(
  NAME ${PROJECT_NAME}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
#  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
