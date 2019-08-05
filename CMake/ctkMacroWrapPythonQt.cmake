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

#! \ingroup CMakeUtilities
macro(ctkMacroWrapPythonQt WRAPPING_NAMESPACE TARGET SRCS_LIST_NAME SOURCES IS_WRAP_FULL HAS_DECORATOR)

  # Sanity check
  if(IS_WRAP_FULL)
    message(FATAL_ERROR "IS_WRAP_FULL option is not supported anymore. See https://github.com/commontk/CTK/issues/449")
  endif()

  # TODO: this find package seems not to work when called form a superbuild, but the call is needed
  # in general to find the python interpreter.  In CTK, the toplevel CMakeLists.txt does the find
  # package so this is a no-op.  Other uses of this file may need to have this call so it is still enabled.
  if(NOT PYTHONINTERP_FOUND)
    find_package(PythonInterp)
    if(NOT PYTHONINTERP_FOUND)
      message(FATAL_ERROR "PYTHON_EXECUTABLE not specified or inexistent when calling ctkMacroWrapPythonQt")
    endif()
  endif()

  set(SOURCES_TO_WRAP)

  # For each class
  foreach(FILE ${SOURCES})

    set(skip_wrapping FALSE)

    if(NOT skip_wrapping)
      # Skip wrapping if file is NOT regular header
      if(NOT ${FILE} MATCHES "^.*\\.[hH]$")
        set(skip_wrapping TRUE)
        if(verbose)
          message("${FILE}: skipping - Not a regular header")
        endif()
      endif()
    endif()

    if(NOT skip_wrapping)
      # Skip wrapping if file is a pimpl header
      if(${FILE} MATCHES "^.*_[pP]\\.[hH]$")
        set(skip_wrapping TRUE)
        if(verbose)
          message("${FILE}: skipping - Pimpl header (*._p.h)")
        endif()
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
        if(verbose)
          message("${FILE}: skipping - WRAP_EXCLUDE")
        endif()
      endif()
    endif()

    # if we should wrap it
    if(NOT skip_wrapping)

      # compute the input filename
      if(IS_ABSOLUTE ${FILE})
        set(TMP_INPUT ${FILE})
      else()
        set(TMP_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
      endif()

      list(APPEND SOURCES_TO_WRAP ${TMP_INPUT})
    endif()
  endforeach()

  # Convert wrapping namespace to subdir
  string(REPLACE "." "_" WRAPPING_NAMESPACE_UNDERSCORE ${WRAPPING_NAMESPACE})

  # Define wrap type and wrap intermediate directory
  set(wrap_int_dir generated_cpp/${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}/)

  set(wrapper_module_init_cpp_filename ${wrap_int_dir}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_module_init.cpp)

  # Configure 'ctkMacroWrapPythonQtModuleInit.cpp.in' using TARGET, HAS_DECORATOR and
  # WRAPPING_NAMESPACE_UNDERSCORE.
  set(TARGET_CONFIG ${TARGET})
  configure_file(
    ${CTK_CMAKE_DIR}/ctkMacroWrapPythonQtModuleInit.cpp.in
    ${wrapper_module_init_cpp_filename}
    @ONLY
    )

  set(extra_args)
  if(verbose)
    set(extra_args --extra-verbose)
  endif()

  # Custom command allow to generate ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp and
  # associated wrappers ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}.cpp
  set(wrapper_init_cpp_filename ${wrap_int_dir}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp)
  set(wrapper_h_filename ${wrap_int_dir}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}.h)
  add_custom_command(
    OUTPUT
      ${wrapper_init_cpp_filename}
      ${wrapper_h_filename}
    DEPENDS
      ${SOURCES_TO_WRAP}
      ${CTK_CMAKE_DIR}/ctkWrapPythonQt.py
    COMMAND ${PYTHON_EXECUTABLE} ${CTK_CMAKE_DIR}/ctkWrapPythonQt.py
      --target=${TARGET}
      --namespace=${WRAPPING_NAMESPACE}
      --output-dir=${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir} ${extra_args}
      ${SOURCES_TO_WRAP}
    COMMENT "PythonQt Wrapping - Generating ${wrapper_init_cpp_filename}"
    VERBATIM
    )
  if(CTK_QT_VERSION VERSION_GREATER "4")
    qt5_wrap_cpp(${TARGET}_MOC_CXX ${CMAKE_CURRENT_BINARY_DIR}/${wrapper_h_filename})
  else()
    QT4_WRAP_CPP(${TARGET}_MOC_CXX ${CMAKE_CURRENT_BINARY_DIR}/${wrapper_h_filename})
  endif()

  # The following files are generated
  set_source_files_properties(
    ${wrapper_init_cpp_filename}
    ${wrapper_h_filename}
    ${wrapper_module_init_cpp_filename}
    PROPERTIES GENERATED TRUE)

  # Create the Init File
  set(${SRCS_LIST_NAME}
    ${${SRCS_LIST_NAME}}
    ${wrapper_init_cpp_filename}
    ${wrapper_module_init_cpp_filename}
    ${${TARGET}_MOC_CXX}
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

