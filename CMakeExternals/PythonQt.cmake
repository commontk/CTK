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

  set(qtlibs core gui multimedia network opengl sql svg uitools xml)

  # Enable Qt libraries PythonQt wrapping if required
  if(CTK_QT_VERSION VERSION_EQUAL "5")
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
  elseif(CTK_QT_VERSION VERSION_EQUAL "6")
    list(APPEND ep_cache_args
      -DQt6_DIR:PATH=${Qt6_DIR}
      )
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
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

  # Variable expected by FindPython3 CMake module
  set(Python3_INCLUDE_DIR ${PYTHON_INCLUDE_DIR})
  set(Python3_LIBRARY ${PYTHON_LIBRARY})
  set(Python3_LIBRARY_DEBUG ${PYTHON_LIBRARY})
  set(Python3_LIBRARY_RELEASE ${PYTHON_LIBRARY})
  find_package(Python3 COMPONENTS Development REQUIRED)

  ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
  if(CTK_QT_VERSION VERSION_EQUAL "5")
    set(revision_tag 606939f5e3883ad3ec2c4ed90d5c97190eb00571) # patched-v3.6.1-2025-09-30-f4769f190
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
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
    set(location_args GIT_REPOSITORY "https://github.com/commontk/PythonQt.git"
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
      # FindPython3
      -DPython3_INCLUDE_DIR:PATH=${Python3_INCLUDE_DIR}
      -DPython3_LIBRARY:FILEPATH=${Python3_LIBRARY}
      -DPython3_LIBRARY_DEBUG:FILEPATH=${Python3_LIBRARY}
      -DPython3_LIBRARY_RELEASE:FILEPATH=${Python3_LIBRARY}
      ${ep_PythonQt_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(PYTHONQT_INSTALL_DIR ${ep_install_dir})

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${CMAKE_BINARY_DIR}/${proj}/<CMAKE_CFG_INTDIR>)
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )

  #-----------------------------------------------------------------------------
  # Launcher setting specific to install tree

  # NA

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
