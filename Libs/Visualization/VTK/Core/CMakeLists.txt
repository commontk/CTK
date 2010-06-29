PROJECT(CTKVisualizationVTKCore)

#
# 3rd party dependencies
#

FIND_PACKAGE(VTK)
IF(NOT VTK_FOUND)
  MESSAGE(FATAL_ERROR "error: VTK package is required to build ${PROJECT_NAME}")
ENDIF()
INCLUDE(${VTK_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_VISUALIZATION_VTK_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  ctkVTKColorTransferFunction.cpp
  ctkVTKColorTransferFunction.h
  ctkVTKCommandOptions.cpp
  ctkVTKCommandOptions.h
  ctkVTKCompositeFunction.cpp
  ctkVTKCompositeFunction.h
  ctkVTKConnection.cpp
  ctkVTKConnection.h
  ctkVTKHistogram.cpp
  ctkVTKHistogram.h
  ctkVTKLookupTable.cpp
  ctkVTKLookupTable.h
  ctkVTKObject.h
  ctkVTKObjectEventsObserver.cpp
  ctkVTKObjectEventsObserver.h
  ctkVTKPiecewiseFunction.cpp
  ctkVTKPiecewiseFunction.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkVTKColorTransferFunction.h
  ctkVTKConnection.h
  ctkVTKCompositeFunction.h
  ctkVTKLookupTable.h
  ctkVTKHistogram.h
  ctkVTKObjectEventsObserver.h
  ctkVTKPiecewiseFunction.h
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
  vtkFiltering
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
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
