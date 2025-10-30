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
    set(_qt_version_string "${Qt5_VERSION_MAJOR}.${Qt5_VERSION_MINOR}.${Qt5_VERSION_PATCH}")
  elseif(CTK_QT_VERSION VERSION_EQUAL "6")
    list(APPEND ep_cache_args
      -DQt6_DIR:PATH=${Qt6_DIR}
      )
    set(_qt_version_string "${Qt6_VERSION_MAJOR}.${Qt6_VERSION_MINOR}.${Qt6_VERSION_PATCH}")
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()

  # Enable PythonQt wrappers
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

  set(revision_tag 4d4800bea3559222c76433d5974c75a01dbec728) # patched-v3.6.1-2025-09-30-f4769f190
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

  set(PythonQt_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
  ExternalProject_Add(${proj}-source
    SOURCE_DIR ${PythonQt_SOURCE_DIR}
    ${location_args}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
  ExternalProject_Message(${proj} "${proj} - Adding ${proj}-source")

  set(PythonQtGenerator_SOURCE_DIR ${PythonQt_SOURCE_DIR}/generator)
  set(PythonQtGenerator_BINARY_DIR ${CMAKE_BINARY_DIR}/PythonQtGenerator-build)
  ExternalProject_Add(PythonQtGenerator
    SOURCE_DIR ${PythonQtGenerator_SOURCE_DIR}
    BINARY_DIR ${PythonQtGenerator_BINARY_DIR}
    PREFIX PythonQtGenerator${ep_suffix}
    DOWNLOAD_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DPythonQtGenerator_QT_VERSION:STRING=${CTK_QT_VERSION}
    INSTALL_COMMAND ""
    DEPENDS
      ${proj}-source
      ${${proj}_DEPENDENCIES}
  )

  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  set(PythonQtGenerator_EXECUTABLE "${PythonQtGenerator_BINARY_DIR}/$<$<BOOL:${_isMultiConfig}>:$<CONFIG>/>PythonQtGenerator${CMAKE_EXECUTABLE_SUFFIX}")

  set(PythonQtGenerator_OUTPUT_DIR ${CMAKE_BINARY_DIR}/PythonQtGenerator-output-${_qt_version_string})

  if(POLICY CMP0114)
    cmake_policy(SET CMP0114 NEW) # CMake 3.19
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${PythonQt_SOURCE_DIR}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    DOWNLOAD_COMMAND ""
    BUILD_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DPythonQt_QT_VERSION:STRING=${CTK_QT_VERSION}
      -DPythonQt_GENERATED_PATH:PATH=${PythonQtGenerator_OUTPUT_DIR}/generated_cpp
      # FindPython3
      -DPython3_INCLUDE_DIR:PATH=${Python3_INCLUDE_DIR}
      -DPython3_LIBRARY:FILEPATH=${Python3_LIBRARY}
      -DPython3_LIBRARY_DEBUG:FILEPATH=${Python3_LIBRARY}
      -DPython3_LIBRARY_RELEASE:FILEPATH=${Python3_LIBRARY}
      ${ep_PythonQt_args}
    DEPENDS
      ${proj}-source
      PythonQtGenerator
      ${${proj}_DEPENDENCIES}
    )
  set(PYTHONQT_INSTALL_DIR ${ep_install_dir})

  ExternalProject_Message(${proj} "${proj} - Adding GenerateWrapper step")
  ExternalProject_Add_Step(${proj} GenerateWrapper
    COMMAND ${PythonQtGenerator_EXECUTABLE} --output-directory=${PythonQtGenerator_OUTPUT_DIR}
    COMMENT "Generating PythonQt wrapper for Qt ${_qt_version_string}"
    DEPENDERS configure
    WORKING_DIRECTORY ${PythonQtGenerator_BINARY_DIR}
    )

  ExternalProject_Add_StepTargets(${proj} GenerateWrapper)

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
