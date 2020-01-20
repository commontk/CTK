project(CTKVisualizationVTKCore)

#
# 3rd party dependencies
#

if(${VTK_VERSION} VERSION_LESS "8.90")
  include(${VTK_USE_FILE})
else()
  # VTK >= 8.90 defines VTK_MAJOR_VERSION
  set(VTK_VERSION_MAJOR ${VTK_MAJOR_VERSION})

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
      WrappingPythonCore
      ViewsContext2D
    OPTIONAL_COMPONENTS
      TestingRendering
    )
endif()

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

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkVTKConnection.h
  ctkVTKObjectEventsObserver.h
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
if(${VTK_VERSION} VERSION_EQUAL 8.90 OR ${VTK_VERSION} VERSION_GREATER 8.90 )
  # VTK_LIBRARIES is automatically set based on selected components
elseif(${VTK_VERSION_MAJOR} GREATER 5)
  set(VTK_LIBRARIES
    # a few of these have to be specified explicitly to workaround
    # underlinked libraries from the debian VTK package
    # see Debian bug #747436
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonSystem
    vtkFiltersStatistics
    vtkIOImage
    vtkInteractionStyle
    vtkRenderingAnnotation
    vtkRenderingCore
    )
  if(TARGET vtkRendering${VTK_RENDERING_BACKEND})
    list(APPEND VTK_LIBRARIES vtkRendering${VTK_RENDERING_BACKEND})
  endif()
  if(TARGET vtkIOMPIImage)
    list(APPEND VTK_LIBRARIES vtkIOMPIImage )
  endif()
  if(TARGET vtkRenderingMatplotlib)
    list(APPEND VTK_LIBRARIES vtkRenderingMatplotlib )
  endif()
  if(TARGET vtkTestingRendering)
    list(APPEND VTK_LIBRARIES vtkTestingRendering )
  endif()
  # With VTK > 6.2.0 (commit 4f7460a5), vtkRenderingFreeTypeOpenGL has been removed.
  if(TARGET vtkRenderingFreeTypeOpenGL)
    list(APPEND VTK_LIBRARIES vtkRenderingFreeTypeOpenGL)
  endif()
  if(TARGET vtkRenderingFreeType )
    list(APPEND VTK_LIBRARIES vtkRenderingFreeType)
  endif()
  if (TARGET vtkRenderingFreeTypeFontConfig AND UNIX AND NOT APPLE)
      list(APPEND VTK_LIBRARIES
        vtkRenderingFreeTypeFontConfig
        )
  endif()
else()
  set(VTK_LIBRARIES
    vtkCommon
    vtkFiltering
    vtkRendering
    vtkHybrid
    )
endif()

if(CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS)
  set(CTK_USE_CHARTS 1)
endif()

if(${CTK_USE_CHARTS})
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
  if(${VTK_VERSION} VERSION_EQUAL 8.90 OR ${VTK_VERSION} VERSION_GREATER 8.90 )
    # VTK_LIBRARIES is automatically set based on selected components
  elseif(${VTK_VERSION_MAJOR} GREATER 5)
    set(VTK_LIBRARIES
      vtkChartsCore
      ${VTK_LIBRARIES})
  else()
    set(VTK_LIBRARIES
      vtkCharts
      ${VTK_LIBRARIES})
  endif()
  add_definitions(-DCTK_USE_CHARTS)

  if(${VTK_VERSION} VERSION_LESS "8.90")
  set(VTK_LIBRARIES
    vtkViewsContext2D
    ${VTK_LIBRARIES})
  if(TARGET vtkRenderingContext${VTK_RENDERING_BACKEND})
    list(APPEND VTK_LIBRARIES vtkRenderingContext${VTK_RENDERING_BACKEND})
  endif()
  endif()

endif()

if(${VTK_VERSION} VERSION_LESS "8.90")

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
  list(APPEND VTK_LIBRARIES ${PYTHON_LIBRARY} ${PYTHONQT_LIBRARIES})
  if(${VTK_VERSION_MAJOR} GREATER 5)
    list(APPEND VTK_LIBRARIES vtkWrappingPythonCore)
  elseif(${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR} VERSION_GREATER 5.6)
    list(APPEND VTK_LIBRARIES vtkPythonCore)
  else()
    list(APPEND VTK_LIBRARIES vtkCommonPythonD)
  endif()
endif()

if(Q_WS_X11)
  # If the faster 'gold' linker is used, to avoid complaints about undefined symbol
  # 'XGetWindowAttributes', 'XCreateWindow', ..., let's link against X11 libraries.
  find_package(X11)
  list(APPEND VTK_LIBRARIES ${X11_LIBRARIES})
endif()

endif()

# Custom export content
if(${VTK_VERSION} VERSION_LESS "8.90")
  set(CTK_VTK_EXPORT_CUSTOM_CONTENT "")
else()
  set(CTK_VTK_EXPORT_CUSTOM_CONTENT "#ifndef VTK_OVERRIDE
#define VTK_OVERRIDE override;
#endif
")
endif()

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries} ${PYTHONQT_LIBRARIES}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  EXPORT_CUSTOM_CONTENT_FROM_VARIABLE "CTK_VTK_EXPORT_CUSTOM_CONTENT"
  )

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

  if (NOT VTK_VERSION VERSION_LESS "8.90.0")
    vtk_module_autoinit(
      TARGETS ${PROJECT_NAME}
      MODULES ${VTK_LIBRARIES}
      )
  endif()

# Plugins
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
