#
# PythonQt
#

superbuild_include_once()

set(PythonQt_enabling_variable PYTHONQT_LIBRARIES)
set(${PythonQt_enabling_variable}_INCLUDE_DIRS PYTHONQT_INCLUDE_DIR PYTHON_INCLUDE_DIRS)
set(${PythonQt_enabling_variable}_FIND_PACKAGE_CMD PythonQt)

set(PythonQt_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(PythonQt)
set(proj PythonQt)

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

  # Enable Qt libraries PythonQt wrapping if required
  set(qtlibs core gui network opengl sql svg uitools webkit xml)
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
  find_package(PythonLibs)
  if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR "error: Python is required to build ${PROJECT_NAME}")
  endif()

  set(revision_tag e1f1c77d9675c3c5fb1cba19d2a32ace483eda2c)
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
    set(location_args GIT_REPOSITORY "${git_protocol}://github.com/commontk/PythonQt.git"
                      GIT_TAG ${revision_tag})
  endif()

  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    CMAKE_GENERATOR ${gen}
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    LIST_SEPARATOR ${sep}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
      -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
      ${ep_PythonQt_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(PYTHONQT_INSTALL_DIR ${ep_install_dir})

  # Since the full path of PythonQt library is used, there is not need to add
  # its corresponding library output directory to CTK_EXTERNAL_LIBRARY_DIRS

else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS
  PYTHONQT_INSTALL_DIR:PATH
  PYTHON_EXECUTABLE:FILEPATH # FindPythonInterp expects PYTHON_EXECUTABLE variable to be defined
  PYTHON_INCLUDE_DIR:PATH # FindPythonQt expects PYTHON_INCLUDE_DIR variable to be defined
  PYTHON_LIBRARY:FILEPATH # FindPythonQt expects PYTHON_LIBRARY variable to be defined
  )
