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
    list(APPEND ep_PythonQt_args
      -DQt6_DIR:PATH=${Qt6_DIR}
      )
    set(_qt_version_string "${Qt6_VERSION_MAJOR}.${Qt6_VERSION_MINOR}.${Qt6_VERSION_PATCH}")
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()

  set(ep_PythonQtGenerator_args ${ep_PythonQt_args})

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

  set(revision_tag ef4ebfbae0620e688c9732baeb7ae09fc2f75dc2) # patched-v3.6.1-2025-12-22-469f01f6a
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
      ${ep_PythonQtGenerator_args}
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

  # Qt runtime (DLL) directory to put on PATH while running the generator.
  get_property(_qt_core_filepath TARGET "Qt${CTK_QT_VERSION}::Core" PROPERTY LOCATION_RELEASE)
  get_filename_component(_qtCore_library_dir ${_qt_core_filepath} PATH)

  # Determine the correct PATH variable and path separator based on the operating system.
  if(WIN32)
    set(PATHVAR_CONFIG "PATH")
    set(_pathsep ";")
  elseif(APPLE)
    set(PATHVAR_CONFIG "DYLD_LIBRARY_PATH")
    set(_pathsep ":")
  else()
    set(PATHVAR_CONFIG "LD_LIBRARY_PATH")
    set(_pathsep ":")
  endif()

  # Launcher script that:
  #  - Extends PATH with the Qt bin dir
  #  - Runs the generator
  # By default, standard output and standard error will be captured and written into
  # two files and displayed only if exit code is not zero:
  #   <CMAKE_BINARY_DIR>/<proj>_<stepname>_step_output.txt
  #   <CMAKE_BINARY_DIR>/<proj>_<stepname>_step_error.txt
  #
  # Logging to files helps prevent CTest or IDE like Visual Studio for improperly
  # identifying process output as errors or warnings.
  #
  # For debugging purpose, logging to files of standard error and output
  # can be disabled setting the environment variable 'EP_EXECUTE_DISABLE_CAPTURE_OUTPUTS'
  # to 1.
  #
  # If the command executes successfully, the following message will be printed:
  #   <proj>: '<stepname>' step successfully completed.
  #
  # In case of error, path to the two log files and their content will be displayed.
  #
  # Adapted from Slicer/CMake/ExternalProjectForNonCMakeProject.cmake
  file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/PythonQtGenerator-launcher.cmake"
     CONTENT [==[
set(ENV{@PATHVAR_CONFIG@} "@_qtCore_library_dir@@_pathsep@$ENV{@PATHVAR_CONFIG@}")

set(proj "@proj@")
set(stepname "GenerateWrapper")

# Check if output and error should be captured into files
set(capture_outputs 1)
if("$ENV{EP_EXECUTE_DISABLE_CAPTURE_OUTPUTS}")
  set(capture_outputs 0)
  set(_reason " (EP_EXECUTE_DISABLE_CAPTURE_OUTPUTS env. variable set to '$ENV{EP_EXECUTE_DISABLE_CAPTURE_OUTPUTS}')")
  message(STATUS "${proj}: '${stepname}' Disabling capture of outputs${_reason}")
endif()

# Execute command
set(_args)
if(capture_outputs)
  set(_args
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    )
endif()
execute_process(
  COMMAND
    ${PythonQtGenerator_EXECUTABLE}
    --output-directory=@PythonQtGenerator_OUTPUT_DIR@
  WORKING_DIRECTORY "@PythonQtGenerator_BINARY_DIR@"
  RESULT_VARIABLE result
  ${_args}
  )

# If it applies, write output to files
if(capture_outputs)
  set(output_file "@CMAKE_BINARY_DIR@/${proj}_${stepname}_step_output.txt")
  file(WRITE ${output_file} ${output})

  set(error_file "@CMAKE_BINARY_DIR@/${proj}_${stepname}_step_error.txt")
  file(WRITE ${error_file} ${error})
endif()

if(NOT ${result} EQUAL 0)
  if(capture_outputs)
    message(STATUS "${proj}: Errors detected - See below.\n${output}\n${error}")
    message(FATAL_ERROR "${proj}: ${stepname} step failed with exit code '${result}'.
Outputs also captured in ${output_file} and ${error_file}.
Setting env. variable EP_EXECUTE_DISABLE_CAPTURE_OUTPUTS to 1 allows to disable file capture.
")
  else()
    message(FATAL_ERROR "${proj}: ${stepname} step failed with exit code '${result}'.")
  endif()
endif()

message(STATUS "${proj}: '${stepname}' step successfully completed.")
]==]
     @ONLY
     )

  ExternalProject_Message(${proj} "${proj} - Adding GenerateWrapper step")
  ExternalProject_Add_Step(${proj} GenerateWrapper
    COMMAND ${CMAKE_COMMAND}
      -DPythonQtGenerator_EXECUTABLE:PATH=${PythonQtGenerator_EXECUTABLE}
      -P ${CMAKE_BINARY_DIR}/PythonQtGenerator-launcher.cmake
    COMMENT "Generating PythonQt wrapper for Qt ${_qt_version_string}"
    USES_TERMINAL 1
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
