PROJECT(CTKVisualizationVTKWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_VISUALIZATION_VTK_WIDGETS_EXPORT")


# Source files
SET(KIT_SRCS
  ctkVTKAbstractMatrixWidget.cpp
  ctkVTKAbstractMatrixWidget.h
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKAbstractView.cpp
  ctkVTKAbstractView.h
  ctkVTKAbstractView_p.h
  ctkVTKDataSetModel.cpp
  ctkVTKDataSetModel.h
  ctkVTKDataSetArrayComboBox.cpp
  ctkVTKDataSetArrayComboBox.h
  ctkVTKMagnifyView.cpp
  ctkVTKMagnifyView.h
  ctkVTKMagnifyView_p.h
  ctkVTKMatrixWidget.cpp
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.cpp
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.cpp
  ctkVTKScalarBarWidget.h
  ctkVTKScalarsToColorsUtils.cpp
  ctkVTKScalarsToColorsUtils.h
  ctkVTKSliceView.cpp
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKSurfaceMaterialPropertyWidget.cpp
  ctkVTKSurfaceMaterialPropertyWidget.h
  ctkVTKTextPropertyWidget.cpp
  ctkVTKTextPropertyWidget.h
  ctkVTKThresholdWidget.cpp
  ctkVTKThresholdWidget.h
  ctkVTKThumbnailView.cpp
  ctkVTKThumbnailView.h
  ctkVTKWidgetsUtils.cpp
  ctkVTKWidgetsUtils.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKAbstractView.h
  ctkVTKAbstractView_p.h
  ctkVTKDataSetArrayComboBox.h
  ctkVTKDataSetModel.h
  ctkVTKMagnifyView.h
  ctkVTKMagnifyView_p.h
  ctkVTKMatrixWidget.h
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.h
  ctkVTKSliceView.h
  ctkVTKSliceView_p.h
  ctkVTKSurfaceMaterialPropertyWidget.h
  ctkVTKTextPropertyWidget.h
  ctkVTKThresholdWidget.h
  ctkVTKThumbnailView.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/ctkVTKScalarBarWidget.ui
  Resources/UI/ctkVTKTextPropertyWidget.ui
  Resources/UI/ctkVTKThresholdWidget.ui
)

# Resources
SET(KIT_resources
  Resources/ctkVTKWidgets.qrc
)

# Set VTK_LIBRARIES variable
SET(VTK_LIBRARIES
  QVTK
  )

IF(CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS)
  SET(CTK_USE_CHARTS 1)
ENDIF()

IF (${CTK_USE_CHARTS})
  SET(KIT_SRCS
      ctkVTKChartView.cpp
      ctkVTKChartView.h
      ctkVTKVolumePropertyWidget.cpp
      ctkVTKVolumePropertyWidget.h
      ctkVTKScalarsToColorsView.cpp
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.cpp
      ctkVTKScalarsToColorsWidget.h
      ${KIT_SRCS})
  SET(KIT_MOC_SRCS
      ctkVTKChartView.h
      ctkVTKVolumePropertyWidget.h
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.h
      ${KIT_MOC_SRCS})
  SET(KIT_UI_FORMS
      Resources/UI/ctkVTKVolumePropertyWidget.ui
      Resources/UI/ctkVTKScalarsToColorsWidget.ui
      ${KIT_UI_FORMS})
  SET(VTK_LIBRARIES
      vtkCharts
      ${VTK_LIBRARIES})
  ADD_DEFINITIONS(-DCTK_USE_CHARTS)
ENDIF(${CTK_USE_CHARTS})

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

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

IF(CTK_WRAP_PYTHONQT_FULL OR CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
ENDIF()

# Plugins
IF(BUILD_QTDESIGNER_PLUGINS)
  ADD_SUBDIRECTORY(Plugins)
ENDIF()

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
