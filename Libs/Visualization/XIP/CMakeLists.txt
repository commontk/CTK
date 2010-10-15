PROJECT(CTKVisualizationXIP)

#
# 3rd party dependencies
#

# FIND_PACKAGE(XIP)
# IF(NOT XIP_FOUND)
#   MESSAGE(FATAL_ERROR "error: XIP package is required to build ${PROJECT_NAME}")
# ENDIF()

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_VISUALIZATION_XIP_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkXIPAdaptor.cpp
  ctkXIPAdaptor.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkXIPAdaptor.h
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

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  #ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
