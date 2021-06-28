#
# PythonQt
#

set(proj PythonQt)

set(${proj}_DEPENDENCIES "")

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED PYTHONQT_INSTALL_DIR AND NOT EXISTS ${PYTHONQT_INSTALL_DIR})
  message(FATAL_ERROR "PYTHONQT_INSTALL_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED PYTHONQT_INSTALL_DIR)

  set(ep_PythonQt_args)

  # Should PythonQt use VTK
  if(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
    list(APPEND proj_DEPENDENCIES VTK)
  endif()

  set(qtlibs core gui network opengl sql svg uitools xml)

  # Enable Qt libraries PythonQt wrapping if required
  if(CTK_QT_VERSION VERSION_GREATER "4")
    list(APPEND ep_PythonQt_args
      -DQt5_DIR:PATH=${Qt5_DIR}
      )
    # XXX Backward compatible way
    if(DEFINED CMAKE_PREFIX_PATH)
      list(APPEND ep_PythonQt_args
        -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
        )
    endif()
    list(APPEND qtlibs qml quick)
    if(CTK_QT_VERSION VERSION_LESS "5.6.0")
      list(APPEND qtlibs webkit)
    endif()
  else()
    list(APPEND ep_PythonQt_args
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      )
    list(APPEND qtlibs webkit)
  endif()

  # Set desired qt version for PythonQt
  list(APPEND ep_PythonQt_args -DPythonQt_QT_VERSION:STRING=${CTK_QT_VERSION})

  foreach(qtlib All ${qtlibs})
    string(TOUPPER ${qtlib} qtlib_uppercase)
    list(APPEND ep_PythonQt_args -DPythonQt_Wrap_Qt${qtlib}:BOOL=${CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib_uppercase}})
  endforeach()

  # Force wrap option to ON if WRAP_QTALL was set to ON
  if(${CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QTALL})
    foreach(qtlib ${qtlibs})
      string(TOUPPER ${qtlib} qtlib_uppercase)
      set(CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib_uppercase} ON CACHE BOOL "Enable Scripting/Python/Core Library PYTHONQT_WRAP_QT${qtlib_uppercase} option" FORCE)
    endforeach()
  endif()

  # Python is required
  if(NOT PYTHONLIBS_FOUND)
    find_package(PythonLibs)
    if(NOT PYTHONLIBS_FOUND)
      message(FATAL_ERROR "error: Python is required to build ${PROJECT_NAME}")
    endif()
  endif()

  ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)

  if (CTK_QT_VERSION VERSION_GREATER "4")
    set(revision_tag c306140442feb6e76d4bae672c48cd7f11d9b0cd) # patched-9
  else()
    set(revision_tag 90c08fb0d523622d2de9e7a91f4ef116a66a8801) # patched-5
  endif()

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
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://github.com/commontk/PythonQt.git"
                      GIT_TAG ${revision_tag})
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    BUILD_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DPythonQt_QT_VERSION:STRING=${CTK_QT_VERSION}
      -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
      -DPYTHON_INCLUDE_DIR2:PATH=${PYTHON_INCLUDE_DIR2}
      -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
      ${ep_PythonQt_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(PYTHONQT_INSTALL_DIR ${ep_install_dir})

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

set(PythonQt_DIR ${PYTHONQT_INSTALL_DIR})

mark_as_superbuild(
  VARS
    PYTHONQT_INSTALL_DIR:PATH
    PYTHON_EXECUTABLE:FILEPATH # FindPythonInterp expects PYTHON_EXECUTABLE variable to be defined
    PYTHON_INCLUDE_DIR:PATH # FindPythonQt expects PYTHON_INCLUDE_DIR variable to be defined
    PYTHON_INCLUDE_DIR2:PATH
    PYTHON_LIBRARY:FILEPATH # FindPythonQt expects PYTHON_LIBRARY variable to be defined
  LABELS "FIND_PACKAGE_VARS"
  )
mark_as_superbuild(
  VARS PythonQt_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
