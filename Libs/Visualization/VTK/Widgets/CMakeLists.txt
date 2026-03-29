project(CTKVisualizationVTKWidgets)

#
# 3rd party dependencies
#

# VTK >= 8.90 defines VTK_MAJOR_VERSION
set(VTK_VERSION_MAJOR ${VTK_MAJOR_VERSION})

set(_test_components)
if(BUILD_TESTING)
  list(APPEND _test_components
    TestingRendering
    ImagingGeneral
    )
endif()

find_package(VTK COMPONENTS
  ChartsCore
  CommonCore
  CommonDataModel
  #Context2D
  FiltersCore
  FiltersGeneral
  FiltersModeling
  FiltersSources
  GUISupportQt
  ImagingGeneral
  ImagingStatistics
  IOLegacy
  IOXML
  InfovisCore
  InfovisLayout
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingOpenGL2
  ViewsCore
  ViewsInfovis
  ${_test_components}
)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_VISUALIZATION_VTK_WIDGETS_EXPORT")

#
# Add the libraries QtTesting
#
if(CTK_USE_QTTESTING)
  find_package(QtTesting REQUIRED)
  # Target name changed from "QtTesting" (old) to "qttesting" (new upstream)
  if(TARGET qttesting)
    set(QtTesting_LINK_TARGET qttesting)
  else()
    set(QtTesting_LINK_TARGET QtTesting)
  endif()
  include_directories(
    ${QtTesting_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR} # For ctkConfig.h
    )
endif()

# Source files
set(KIT_SRCS
  ctkVTKAbstractMatrixWidget.cpp
  ctkVTKAbstractMatrixWidget.h
  ctkVTKAbstractMatrixWidget_p.h
  ctkVTKAbstractView.cpp
  ctkVTKAbstractView.h
  ctkVTKAbstractView_p.h
  ctkVTKColorTransferFunction.cpp
  ctkVTKColorTransferFunction.h
  ctkVTKCompositeFunction.cpp
  ctkVTKCompositeFunction.h
  ctkVTKDataSetModel.cpp
  ctkVTKDataSetModel.h
  ctkVTKDataSetArrayComboBox.cpp
  ctkVTKDataSetArrayComboBox.h
  ctkVTKHistogram.cpp
  ctkVTKHistogram.h
  ctkVTKLookupTable.cpp
  ctkVTKLookupTable.h
  ctkVTKMagnifyView.cpp
  ctkVTKMagnifyView.h
  ctkVTKMagnifyView_p.h
  ctkVTKMatrixWidget.cpp
  ctkVTKMatrixWidget.h
  ctkVTKOpenGLNativeWidget.h
  ctkVTKPiecewiseFunction.cpp
  ctkVTKPiecewiseFunction.h
  ctkVTKPropertyWidget.cpp
  ctkVTKPropertyWidget.h
  ctkVTKRenderView.cpp
  ctkVTKRenderView.h
  ctkVTKRenderView_p.h
  ctkVTKScalarBarWidget.cpp
  ctkVTKScalarBarWidget.h
  ctkVTKScalarsToColorsComboBox.cpp
  ctkVTKScalarsToColorsComboBox.h
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

# UI files
set(KIT_UI_FORMS
  Resources/UI/ctkVTKDiscretizableColorTransferWidget.ui
  Resources/UI/ctkVTKScalarBarWidget.ui
  Resources/UI/ctkVTKTextPropertyWidget.ui
  Resources/UI/ctkVTKPropertyWidget.ui
  Resources/UI/ctkVTKThresholdWidget.ui
)

# Resources
set(KIT_resources
  Resources/ctkVTKWidgets.qrc
)

# Set VTK_LIBRARIES variable
# VTK_LIBRARIES is automatically set based on selected components

if(CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS)
  set(KIT_SRCS
      ctkVTKChartView.cpp
      ctkVTKChartView.h
	  ctkVTKDiscretizableColorTransferWidget.h
	  ctkVTKDiscretizableColorTransferWidget.cpp
      ctkVTKVolumePropertyWidget.cpp
      ctkVTKVolumePropertyWidget.h
      ctkVTKScalarsToColorsView.cpp
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.cpp
      ctkVTKScalarsToColorsWidget.h
      ${KIT_SRCS})
  set(KIT_MOC_SRCS
      ctkVTKChartView.h
	  ctkVTKDiscretizableColorTransferWidget.h
      ctkVTKVolumePropertyWidget.h
      ctkVTKScalarsToColorsView.h
      ctkVTKScalarsToColorsWidget.h
      ${KIT_MOC_SRCS})
  set(KIT_UI_FORMS
      Resources/UI/ctkVTKVolumePropertyWidget.ui
      Resources/UI/ctkVTKScalarsToColorsWidget.ui
      ${KIT_UI_FORMS})
  # VTK_LIBRARIES is automatically set based on selected components
endif()

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

# If we use QtTessting, we add all the dependencies
if(CTK_USE_QTTESTING)
  list(APPEND KIT_SRCS
    ctkVTKRenderViewEventPlayer.cpp
    ctkVTKRenderViewEventPlayer.h
    ctkVTKRenderViewEventTranslator.cpp
    ctkVTKRenderViewEventTranslator.h
    )
  list(APPEND KIT_MOC_SRCS
    ctkVTKRenderViewEventPlayer.h
    ctkVTKRenderViewEventTranslator.h
)
  list(APPEND KIT_target_libraries ${QtTesting_LINK_TARGET})
endif()

# Prefer QVTKOpenGLWidget to QVTKWidget when using Qt5
set(_use_qvtkopenglwidget 1)

if(_use_qvtkopenglwidget)
  add_definitions(-DCTK_USE_QVTKOPENGLWIDGET)
endif()

# QVTKOpenGLNativeWidget.h is always available with VTK >= 9
set(_has_QVTKOpenGLNativeWidget_h 1)


if(_has_QVTKOpenGLNativeWidget_h)
  add_definitions(-DCTK_HAS_QVTKOPENGLNATIVEWIDGET_H)
endif()

# Custom export content
set(CTK_VTK_EXPORT_CUSTOM_CONTENT "#ifndef VTK_OVERRIDE
#define VTK_OVERRIDE override;
#endif
")

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  EXPORT_CUSTOM_CONTENT_FROM_VARIABLE "CTK_VTK_EXPORT_CUSTOM_CONTENT"
  )

if(_use_qvtkopenglwidget)
  target_compile_definitions(
    ${PROJECT_NAME}
    INTERFACE
      CTK_USE_QVTKOPENGLWIDGET
    )
endif()

if(_has_QVTKOpenGLNativeWidget_h)
  target_compile_definitions(
    ${PROJECT_NAME}
    INTERFACE
      CTK_HAS_QVTKOPENGLNATIVEWIDGET_H
    )
endif()

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
  )


# Plugins
if(CTK_BUILD_QTDESIGNER_PLUGINS)
  add_subdirectory(Plugins)
endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
