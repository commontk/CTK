###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# ctkMacroWrapPythonQt
#

#!
#! Depends on:
#!  PythonQt
#!  PythonInterp
#!

#!
#! The different parameters are:
#!
#!    WRAPPING_NAMESPACE: Namespace that should contain the library. For example: org.commontk
#!
#!    TARGET ...........: Name of the wrapped library. For example: CTKWidget
#!
#!    SRCS_LIST_NAME ...: Name of the variable that should contain the generated wrapper source.
#!                        For example: KIT_PYTHONQT_SRCS
#!
#!    SOURCES ..........: List of source files that should be wrapped.
#!
#!    HAS_DECORATOR ....: Indicate if a custom PythonQt decorator header is expected.
#!

#!
#! LOG FILE:
#!   File ctkMacroWrapPythonQt_log.txt will be created in the current directory.
#!   It will contain the list of file and the reason why a given class hasn't been wrapped.
#!

set(verbose 0)

#!
#! Convenient function allowing to log the reason why a given class hasn't been wrapped
#! If verbose=1, it will also be displayed on the standard output
#!
#! \ingroup CMakeUtilities
function(ctkMacroWrapPythonQt_log msg)
  if(verbose)
    message(${msg})
  endif()
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/ctkMacroWrapPythonQt_log.txt" "${msg}\n")
endfunction()

#! \ingroup CMakeUtilities
macro(ctkMacroWrapPythonQt WRAPPING_NAMESPACE TARGET SRCS_LIST_NAME SOURCES IS_WRAP_FULL HAS_DECORATOR)

  # Sanity check
  if(IS_WRAP_FULL)
    message(FATAL_ERROR "IS_WRAP_FULL option is not supported anymore. See https://github.com/commontk/CTK/issues/449")
  endif()

  # TODO: this find package seems not to work when called form a superbuild, but the call is needed
  # in general to find the python interpreter.  In CTK, the toplevel CMakeLists.txt does the find
  # package so this is a no-op.  Other uses of this file may need to have this call so it is still enabled.
  find_package(PythonInterp)
  if(NOT PYTHONINTERP_FOUND)
    message(FATAL_ERROR "PYTHON_EXECUTABLE not specified or inexistent when calling ctkMacroWrapPythonQt")
  endif()

  # Extract python lib path
  get_filename_component(PYTHON_DIR_PATH ${PYTHON_EXECUTABLE} PATH)
  set(PYTHON_LIBRARY_PATH ${PYTHON_DIR_PATH}/../lib)
  if(WIN32)
    set(PYTHON_LIBRARY_PATH ${PYTHON_DIR_PATH})
  endif()

  # Clear log file
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ctkMacroWrapPythonQt_log.txt" "")

  # Convert wrapping namespace to subdir
  string(REPLACE "." "_" WRAPPING_NAMESPACE_UNDERSCORE ${WRAPPING_NAMESPACE})

  set(SOURCES_TO_WRAP)
  set(SOURCES_TO_WRAP_ARG) # Custom_command argument

  # For each class
  foreach(FILE ${SOURCES})

    set(skip_wrapping FALSE)

    if(NOT skip_wrapping)
      # Skip wrapping if file is NOT regular header
      if(NOT ${FILE} MATCHES "^.*\\.[hH]$")
        set(skip_wrapping TRUE)
        ctkMacroWrapPythonQt_log("${FILE}: skipping - Not a regular header")
      endif()
    endif()

    if(NOT skip_wrapping)
      # Skip wrapping if file is a pimpl header
      if(${FILE} MATCHES "^.*_[pP]\\.[hH]$")
        set(skip_wrapping TRUE)
        ctkMacroWrapPythonQt_log("${FILE}: skipping - Pimpl header (*._p.h)")
      endif()
    endif()

    if(NOT skip_wrapping)
      # Skip wrapping if file should excluded
      set(skip_wrapping TRUE)
      get_source_file_property(TMP_WRAP_EXCLUDE ${FILE} WRAP_EXCLUDE)
      if(NOT TMP_WRAP_EXCLUDE)
        set(skip_wrapping FALSE)
      endif()
      if(skip_wrapping)
        ctkMacroWrapPythonQt_log("${FILE}: skipping - WRAP_EXCLUDE")
      endif()
    endif()

    # if we should wrap it
    if(NOT skip_wrapping)

      # compute the input filename
      if(IS_ABSOLUTE FILE)
        set(TMP_INPUT ${FILE})
      else()
        set(TMP_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
      endif()

      list(APPEND SOURCES_TO_WRAP ${TMP_INPUT})
      set(SOURCES_TO_WRAP_ARG "${SOURCES_TO_WRAP_ARG}^^${TMP_INPUT}")
    endif()
  endforeach()

  # Define wrap type and wrap intermediate directory
  set(wrap_int_dir generated_cpp/${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}/)
  #message("wrap_int_dir:${wrap_int_dir}")

  set(wrapper_init_cpp_filename ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp)
  set(wrapper_init_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}${wrapper_init_cpp_filename})

  set(wrapper_module_init_cpp_filename ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_module_init.cpp)
  set(wrapper_module_init_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}${wrapper_module_init_cpp_filename})

  # Custom command allow to generate ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp and
  # associated wrappers ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}{0-N}.cpp
  add_custom_command(
    OUTPUT
      ${wrap_int_dir}${wrapper_init_cpp_filename}
      ${wrap_int_dir}${wrapper_module_init_cpp_filename}
    DEPENDS
      ${SOURCES_TO_WRAP}
      ${CTK_CMAKE_DIR}/ctkScriptWrapPythonQt_Light.cmake
      ${CTK_CMAKE_DIR}/ctkMacroWrapPythonQtModuleInit.cpp.in
    COMMAND ${CMAKE_COMMAND}
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
      -DPYTHON_LIBRARY_PATH:PATH=${PYTHON_LIBRARY_PATH}
      -DWRAPPING_SCRIPT:FILEPATH=${CTK_CMAKE_DIR}/ctkWrapPythonQt.py
      -DWRAPPING_NAMESPACE:STRING=${WRAPPING_NAMESPACE}
      -DTARGET:STRING=${TARGET}
      -DSOURCES:STRING=${SOURCES_TO_WRAP_ARG}
      -DOUTPUT_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}
      -DHAS_DECORATOR:BOOL=${HAS_DECORATOR}
      -P ${CTK_CMAKE_DIR}/ctkScriptWrapPythonQt_Light.cmake
    COMMENT "PythonQt Wrapping - Generating ${wrapper_init_cpp_filename}"
    VERBATIM
    )

  # Clear variable
  set(moc_flags)

  # Grab moc flags
  QT4_GET_MOC_FLAGS(moc_flags)

  # Prepare custom_command argument
  set(moc_flags_arg)
  foreach(flag ${moc_flags})
    set(moc_flags_arg "${moc_flags_arg}^^${flag}")
  endforeach()

  # On Windows, to avoid "too long input" error, dump moc flags.
  if(WIN32)
    # File containing the moc flags
    set(wrapper_moc_flags_filename mocflags_${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_all.txt)
    set(wrapper_master_moc_flags_file ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}${wrapper_moc_flags_filename})
    file(WRITE ${wrapper_master_moc_flags_file} ${moc_flags_arg})
    # The arg passed to the custom command will be the file containing the list of moc flags
    set(moc_flags_arg ${wrapper_master_moc_flags_file})
  endif()

  # File to run through moc
  set(wrapper_master_moc_filename moc_${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_all.cpp)
  set(wrapper_master_moc_file ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}${wrapper_master_moc_filename})

  # Custom command allowing to call moc to process the wrapper headers
  add_custom_command(
    OUTPUT ${wrap_int_dir}${wrapper_master_moc_filename}
    DEPENDS
      ${wrap_int_dir}${wrapper_init_cpp_filename}
      ${wrap_int_dir}${wrapper_module_init_cpp_filename}
      ${extra_files} ${CTK_CMAKE_DIR}/ctkScriptMocPythonQtWrapper.cmake
    COMMAND ${CMAKE_COMMAND}
      -DWRAPPING_NAMESPACE:STRING=${WRAPPING_NAMESPACE}
      -DTARGET:STRING=${TARGET}
      -DMOC_FLAGS:STRING=${moc_flags_arg}
      -DWRAP_INT_DIR:STRING=${wrap_int_dir}
      -DWRAPPER_MASTER_MOC_FILE:STRING=${wrapper_master_moc_file}
      -DOUTPUT_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
      -DQT_MOC_EXECUTABLE:FILEPATH=${QT_MOC_EXECUTABLE}
      -P ${CTK_CMAKE_DIR}/ctkScriptMocPythonQtWrapper.cmake
    COMMENT "PythonQt Wrapping - Moc'ing ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET} wrapper headers"
    VERBATIM
    )

  # The following files are generated
  set_source_files_properties(
    ${wrap_int_dir}${wrapper_init_cpp_filename}
    ${wrap_int_dir}${wrapper_module_init_cpp_filename}
    ${wrap_int_dir}${wrapper_master_moc_filename}
    PROPERTIES GENERATED TRUE)

  # Create the Init File
  set(${SRCS_LIST_NAME}
    ${${SRCS_LIST_NAME}}
    ${wrap_int_dir}${wrapper_init_cpp_filename}
    ${wrap_int_dir}${wrapper_module_init_cpp_filename}
    ${wrap_int_dir}${wrapper_master_moc_filename}
    )

  #
  # Let's include the headers associated with PythonQt
  #
  find_package(PythonQt)
  if(NOT PYTHONQT_FOUND)
    message(FATAL_ERROR "error: PythonQt package is required to build ${TARGET}PythonQt")
  endif()
  include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHONQT_INCLUDE_DIR})

endmacro()

