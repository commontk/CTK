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

  
# Source files
SET(KIT_SRCS
  ctkVTKAbstractMatrixWidget.cpp
  ctkVTKAbstractMatrixWidget.h
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKDataSetModel.cpp
  ctkVTKDataSetModel.h
  ctkVTKDataSetArrayComboBox.cpp
  ctkVTKDataSetArrayComboBox.h
  ctkVTKMatrixWidget.cpp
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.cpp
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.cpp
  ctkVTKScalarBarWidget.h
  ctkVTKSliceView.cpp
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKSurfaceMaterialPropertyWidget.cpp
  ctkVTKSurfaceMaterialPropertyWidget.h
  ctkVTKTextPropertyWidget.cpp
  ctkVTKTextPropertyWidget.h
  ctkVTKThumbnailView.cpp
  ctkVTKThumbnailView.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKDataSetArrayComboBox.h
  ctkVTKDataSetModel.h
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.h
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKSurfaceMaterialPropertyWidget.h
  ctkVTKTextPropertyWidget.h
  ctkVTKThumbnailView.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/ctkVTKScalarBarWidget.ui
  Resources/UI/ctkVTKTextPropertyWidget.ui
)

# Resources
SET(KIT_resources

)

# Set VTK_LIBRARIES variable
SET(VTK_LIBRARIES
  QVTK
  )

IF(VTK_VERSION VERSION_GREATER 5.7)
  SET(CTK_USE_CHARTS 1)
ENDIF(VTK_VERSION VERSION_GREATER 5.7)

IF (${CTK_USE_CHARTS})
  SET(KIT_SRCS
      ctkVTKChartView.cpp
      ctkVTKChartView.h
      ctkVTKScalarsToColorsView.cpp
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.cpp
      ctkVTKScalarsToColorsWidget.h
      ${KIT_SRCS})
  SET(KIT_MOC_SRCS 
      ctkVTKChartView.h
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.h
      ${KIT_MOC_SRCS})
  SET(KIT_UI_FORMS
      Resources/UI/ctkVTKScalarsToColorsWidget.ui 
      ${KIT_UI_FORMS})
  SET(VTK_LIBRARIES
      vtkCharts
      ${VTK_LIBRARIES})
  ADD_DEFINITIONS(-DCTK_USE_CHARTS)
ENDIF(${CTK_USE_CHARTS})

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
ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
