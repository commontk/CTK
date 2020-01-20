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

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(VTK_DIR CACHE)
  find_package(VTK REQUIRED NO_MODULE)
endif()

# Sanity checks
if(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  message(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED VTK_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

  set(revision_tag f3f70ec0912cf836a3d5954a95cb04f0237fbdc4)
  if(${proj}_REVISION_TAG)
    set(revision_tag ${${proj}_REVISION_TAG})
  endif()

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  elseif(${proj}_GIT_REPOSITORY)
    set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                      GIT_TAG ${revision_tag})
  else()
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://vtk.org/VTK.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(additional_vtk_cmakevars )
  if(MINGW)
    list(APPEND additional_vtk_cmakevars -DCMAKE_USE_PTHREADS:BOOL=OFF)
  endif()

  if(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
    ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
    list(APPEND additional_vtk_cmakevars
      -DPYTHON_EXECUTABLE:PATH=${PYTHON_EXECUTABLE}
      -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
      -DPYTHON_DEBUG_LIBRARIES:FILEPATH=${PYTHON_DEBUG_LIBRARIES}
      )
  endif()

  if(CTK_QT_VERSION VERSION_LESS "5")
    list(APPEND additional_vtk_cmakevars
      -DVTK_QT_VERSION:STRING=4
      -DDESIRED_QT_VERSION:STRING=4
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      )
  else()
    list(APPEND additional_vtk_cmakevars
      -DVTK_QT_VERSION:STRING=5
      -DDESIRED_QT_VERSION:STRING=5
      -DQt5_DIR:PATH=${Qt5_DIR}
      )
    # XXX Backward compatible way
    if(DEFINED CMAKE_PREFIX_PATH)
      list(APPEND additional_vtk_cmakevars
        -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
        )
    endif()
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
      -DVTK_WRAP_TCL:BOOL=OFF
      -DVTK_USE_TK:BOOL=OFF
      -DVTK_WRAP_PYTHON:BOOL=${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK}
      -DVTK_WRAP_JAVA:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DVTK_USE_GUISUPPORT:BOOL=ON
      -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
      -DVTK_USE_QT:BOOL=ON # VTK 5
      -DVTK_Group_Qt:BOOL=ON # VTK 6
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

if(VTK_PYTHON_VERSION VERSION_GREATER "2.7")
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
endif()
