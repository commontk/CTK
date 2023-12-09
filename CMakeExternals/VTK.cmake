#
# VTK
#

set(proj VTK)

set(${proj}_DEPENDENCIES "")

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

# Let the user to chose whether to use libs installed in the system
option(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} "Use installed ${proj} library in the system" OFF)
mark_as_advanced(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(VTK_DIR CACHE)
  find_package(VTK REQUIRED NO_MODULE)
endif()

# Sanity checks
if(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  message(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED VTK_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

  if(NOT DEFINED CTK_VTK_VERSION_MAJOR)
      set(CTK_VTK_VERSION_MAJOR "9")
  endif()
  if(NOT CTK_VTK_VERSION_MAJOR MATCHES "^(8|9)$")
    message(FATAL_ERROR "Expected value for CTK_VTK_VERSION_MAJOR is either '8' or '9'")
  endif()
  ExternalProject_Message(${proj} "VTK[CTK_VTK_VERSION_MAJOR:${CTK_VTK_VERSION_MAJOR}]")

  if(CTK_VTK_VERSION_MAJOR VERSION_EQUAL "8")
    set(revision_tag v8.0.1)
  elseif(CTK_VTK_VERSION_MAJOR VERSION_EQUAL "9")
    set(revision_tag v9.2.2)
  endif()
  if(${proj}_REVISION_TAG)
    set(revision_tag ${${proj}_REVISION_TAG})
  endif()
  ExternalProject_Message(${proj} "${proj}[revision_tag:${revision_tag}]")

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  elseif(${proj}_GIT_REPOSITORY)
    set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                      GIT_TAG ${revision_tag})
  else()
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://github.com/kitware/VTK.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(additional_vtk_cmakevars )
  set(additional_vtk8_cmakevars )
  set(additional_vtk9_cmakevars )
  if(MINGW)
    list(APPEND additional_vtk_cmakevars -DCMAKE_USE_PTHREADS:BOOL=OFF)
  endif()

  if(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)

    if(NOT DEFINED VTK_PYTHON_VERSION)
      set(VTK_PYTHON_VERSION ${PYTHON_VERSION_MAJOR})
    endif()
    if(NOT "${VTK_PYTHON_VERSION}" STREQUAL "${PYTHON_VERSION_MAJOR}")
      message(FATAL_ERROR "error: VTK_PYTHON_VERSION [${VTK_PYTHON_VERSION}] is expected to match PYTHON_VERSION_MAJOR [${PYTHON_VERSION_MAJOR}]")
    endif()
      set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
      set(Python3_INCLUDE_DIR ${PYTHON_INCLUDE_DIR})
      set(Python3_LIBRARY ${PYTHON_LIBRARY})
      find_package(Python3 COMPONENTS Interpreter Development)

    ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
    list(APPEND additional_vtk_cmakevars
      -DVTK_PYTHON_VERSION:STRING=${VTK_PYTHON_VERSION}
      # FindPythonInterp, FindPythonLibs
      -DPYTHON_EXECUTABLE:PATH=${PYTHON_EXECUTABLE}
      -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
      -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
      -DPYTHON_DEBUG_LIBRARIES:FILEPATH=${PYTHON_DEBUG_LIBRARIES}
      )
      # VTK9
      list(APPEND additional_vtk9_cmakevars
        # FindPython3
        -DPython3_INCLUDE_DIR:PATH=${Python3_INCLUDE_DIR}
        -DPython3_LIBRARY:FILEPATH=${Python3_LIBRARY}
        -DPython3_EXECUTABLE:FILEPATH=${Python3_EXECUTABLE}
        )
  endif()

  if(CTK_QT_VERSION VERSION_EQUAL "5")
    # Qt5
    list(APPEND additional_vtk_cmakevars
      -DQt5_DIR:PATH=${Qt5_DIR}
      )
  elseif(CTK_QT_VERSION VERSION_EQUAL "6")
    # Qt6
    list(APPEND additional_vtk_cmakevars
      -DQt6_DIR:PATH=${Qt6_DIR}
      )
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()

  # VTK 8
  list(APPEND additional_vtk8_cmakevars
    -DModule_vtkChartsCore:BOOL=ON
    -DModule_vtkRenderingContext2D:BOOL=ON
    -DModule_vtkRenderingContextOpenGL2:BOOL=ON
    -DModule_vtkGUISupportQt:BOOL=ON
    -DModule_vtkGUISupportQtOpenGL:BOOL=ON # OpenGL2 rendering backend
    -DModule_vtkTestingRendering:BOOL=ON
    )

  # VTK 9
  list(APPEND additional_vtk9_cmakevars
    -DVTK_MODULE_ENABLE_VTK_ChartsCore:STRING=YES
    -DVTK_MODULE_ENABLE_VTK_ViewsContext2D:STRING=YES
    -DVTK_MODULE_ENABLE_VTK_RenderingContext2D:STRING=YES
    -DVTK_MODULE_ENABLE_VTK_RenderingContextOpenGL2:STRING=YES
    -DVTK_MODULE_ENABLE_VTK_GUISupportQt:STRING=YES
    -DVTK_GROUP_ENABLE_Qt:STRING=YES
    -DVTK_MODULE_ENABLE_VTK_GUISupportQtOpenGL:STRING=YES # OpenGL2 rendering backend
    )
  # XXX Backward compatible way
  if(DEFINED CMAKE_PREFIX_PATH)
    list(APPEND additional_vtk9_cmakevars
      -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
      )
  endif()

  if(UNIX AND NOT APPLE)
    find_package(FontConfig QUIET)
    if(FONTCONFIG_FOUND)
      # VTK8
      list(APPEND additional_vtk8_cmakevars
        -DModule_vtkRenderingFreeTypeFontConfig:BOOL=ON
        )
      # VTK9
      list(APPEND additional_vtk9_cmakevars
        -DVTK_MODULE_ENABLE_VTK_RenderingFreeTypeFontConfig:BOOL=ON
        )
    endif()

    # OpenGL_GL_PREFERENCE
    if(NOT DEFINED OpenGL_GL_PREFERENCE)
      set(OpenGL_GL_PREFERENCE "LEGACY")
    endif()
    if(NOT "${OpenGL_GL_PREFERENCE}" MATCHES "^(LEGACY|GLVND)$")
      message(FATAL_ERROR "OpenGL_GL_PREFERENCE variable is expected to be set to LEGACY or GLVND")
    endif()
    ExternalProject_Message(${proj} "VTK[OpenGL_GL_PREFERENCE:${OpenGL_GL_PREFERENCE}]")
    list(APPEND additional_vtk_cmakevars
      -DOpenGL_GL_PREFERENCE:STRING=${OpenGL_GL_PREFERENCE}
      )
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      ${additional_vtk_cmakevars}
      ${additional_vtk${CTK_VTK_VERSION_MAJOR}_cmakevars}
      -DVTK_WRAP_TCL:BOOL=OFF
      -DVTK_USE_TK:BOOL=OFF
      -DVTK_WRAP_PYTHON:BOOL=${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK}
      -DVTK_WRAP_JAVA:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DVTK_Group_Qt:BOOL=ON
      -DVTK_QT_VERSION:STRING=${CTK_QT_VERSION}
      -DVTK_LEGACY_REMOVE:BOOL=ON
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS VTK_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

  # Propagate variables expected when VTK searches for python
  mark_as_superbuild(
    VARS
      Python3_INCLUDE_DIR:PATH
      Python3_ROOT_DIR:PATH
      Python3_LIBRARY:FILEPATH
      Python3_LIBRARY_DEBUG:FILEPATH
      Python3_LIBRARY_RELEASE:FILEPATH
      Python3_EXECUTABLE:FILEPATH
    )
