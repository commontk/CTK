project(CTKVisualizationVTKCore)

#
# 3rd party dependencies
#

# VTK >= 8.90 defines VTK_MAJOR_VERSION
set(VTK_VERSION_MAJOR ${VTK_MAJOR_VERSION})

set(_test_components)
if(BUILD_TESTING)
  list(APPEND _test_components
    TestingRendering
    CommonSystem
    )
endif()

set(_python_components)
if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  list(APPEND _python_components
    WrappingPythonCore
    )
endif()

find_package(VTK
  COMPONENTS
    ChartsCore
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersGeneral
    FiltersModeling
    FiltersSources
    FiltersStatistics
    IOLegacy
    IOXML
    InfovisCore
    InfovisLayout
    InteractionStyle
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    ViewsCore
    ViewsInfovis
    ViewsContext2D
    ${_python_components}
    ${_test_components}
  )

if(NOT DEFINED VTK_RENDERING_BACKEND)
  set(VTK_RENDERING_BACKEND "OpenGL")
endif()

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  find_package(PythonQt REQUIRED)
endif()

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_VISUALIZATION_VTK_CORE_EXPORT")


# Source files
set(KIT_SRCS
  ctkVTKConnection.cpp
  ctkVTKConnection.h
  ctkVTKErrorLogMessageHandler.cpp
  ctkVTKErrorLogMessageHandler.h
  ctkVTKObject.h
  ctkVTKObjectEventsObserver.cpp
  ctkVTKObjectEventsObserver.h
  ctkVTKScalarsToColorsUtils.cpp
  ctkVTKScalarsToColorsUtils.h
  vtkLightBoxRendererManager.cpp
  vtkLightBoxRendererManager.h
  )

# UI files
set(KIT_UI_FORMS
  )

# Resources
set(KIT_resources
  )

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  list(APPEND KIT_SRCS
    ctkVTKPythonQtWrapperFactory.cpp
    )
endif()

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  include_directories(
    ${PYTHON_INCLUDE_DIRS}
    ${PYTHONQT_INCLUDE_DIR}
    )
endif()

# Set VTK_LIBRARIES variable
# VTK_LIBRARIES is automatically set based on selected components

if(CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS)
  set(KIT_SRCS
    vtkDiscretizableColorTransferChart.cpp
    vtkDiscretizableColorTransferChart.h
    vtkDiscretizableColorTransferControlPointsItem.cpp
    vtkDiscretizableColorTransferControlPointsItem.h
    vtkScalarsToColorsContextItem.cpp
    vtkScalarsToColorsContextItem.h
    vtkScalarsToColorsHistogramChart.cpp
    vtkScalarsToColorsHistogramChart.h
    vtkScalarsToColorsPreviewChart.cpp
    vtkScalarsToColorsPreviewChart.h
    ${KIT_SRCS})
  # VTK_LIBRARIES is automatically set based on selected components

endif()

# Custom export content
set(CTK_VTK_EXPORT_CUSTOM_CONTENT "#ifndef VTK_OVERRIDE
#define VTK_OVERRIDE override;
#endif
")

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries} ${PYTHONQT_LIBRARIES}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  EXPORT_CUSTOM_CONTENT_FROM_VARIABLE "CTK_VTK_EXPORT_CUSTOM_CONTENT"
  )

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    $<$<BOOL:${CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS}>:CTK_USE_CHARTS>
  )

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
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
