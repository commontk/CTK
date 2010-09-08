PROJECT(CTKVisualizationVTKWidgets)

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

SET(KIT_export_directive "CTK_VISUALIZATION_VTK_WIDGETS_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  ctkVTKAbstractMatrixWidget.cpp
  ctkVTKAbstractMatrixWidget.h
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKChartView.cpp
  ctkVTKChartView.h
  ctkVTKMatrixWidget.cpp
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.cpp
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.cpp
  ctkVTKScalarBarWidget.h
  ctkVTKScalarsToColorsView.cpp
  ctkVTKScalarsToColorsView.h
  ctkVTKScalarsToColorsWidget.cpp
  ctkVTKScalarsToColorsWidget.h
  ctkVTKSliceView.cpp
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKTextPropertyWidget.cpp
  ctkVTKTextPropertyWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKChartView.h
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.h
  ctkVTKScalarsToColorsView.h
  ctkVTKScalarsToColorsWidget.h
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKTextPropertyWidget.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/ctkVTKScalarBarWidget.ui
  Resources/UI/ctkVTKScalarsToColorsWidget.ui
  Resources/UI/ctkVTKTextPropertyWidget.ui
)

# Resources
SET(KIT_resources

)

# Set VTK_LIBRARIES variable
SET(VTK_LIBRARIES
  QVTK
  vtkCharts
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
ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
