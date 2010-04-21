PROJECT(CTKVisualizationVTKCore)

#
# 3rd party dependencies
#

FIND_PACKAGE(VTK)
# TODO In case VTK is passed directly using VTK_DIR, check expected options
IF(NOT VTK_FOUND)
  MESSAGE(FATAL_ERROR "error: VTK package is required to build ${PROJECT_NAME}")
ENDIF()
INCLUDE(${VTK_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildQtLib.cmake for details
#

SET(KIT_export_directive "CTK_VISUALIZATION_VTK_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  ctkVTKCommandOptions.cpp
  ctkVTKCommandOptions.h
  ctkVTKConnection.cpp
  ctkVTKConnection.h
  ctkVTKObject.h
  ctkVTKObjectEventsObserver.cpp
  ctkVTKObjectEventsObserver.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkVTKConnection.h
  ctkVTKObjectEventsObserver.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Set VTK_LIBRARIES variable
SET(VTK_LIBRARIES
  vtkCommon
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildQtLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
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
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
