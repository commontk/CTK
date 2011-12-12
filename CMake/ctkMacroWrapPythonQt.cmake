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
#!  PythonQtGenerator (Only if IS_WRAP_FULL is TRUE)
#!  PythonInterp (See function reSearchFile)
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
#!    IS_WRAP_FULL .....: Indicate if a Full wrapping if desired.
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

include(${CTK_CMAKE_DIR}/ctkMacroSetPaths.cmake)

#!
#! Convenient function allowing to invoke re.search(regex, string) using the given interpreter.
#! Note that is_matching will be set to True if there is a match
#!
#! \ingroup CMakeUtilities
function(ctkMacroWrapPythonQt_reSearchFile python_exe python_library_path regex file is_matching)

  set(python_cmd "import re
f = open('${file}', 'r')
res = re.search('${regex}', f.read(), re.MULTILINE)
if res == None: print 'FALSE'
else: print 'TRUE'
")
  #message("python_cmd: ${python_cmd}")

  ctkMacroSetPaths("${python_library_path}")
  execute_process(
    COMMAND ${python_exe} -c ${python_cmd}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  if(result)
    message(FATAL_ERROR "reSearchFile - Problem with regex: ${regex}\n${error}\nPython exe: ${python_exe}\nPython lib path: ${python_library_path}")
  endif()
  set(is_matching ${output} PARENT_SCOPE)
endfunction()

#! \ingroup CMakeUtilities
macro(ctkMacroWrapPythonQt WRAPPING_NAMESPACE TARGET SRCS_LIST_NAME SOURCES IS_WRAP_FULL HAS_DECORATOR)

  # Sanity check
  if(IS_WRAP_FULL AND NOT EXISTS "${PYTHONQTGENERATOR_EXECUTABLE}")
    message(FATAL_ERROR "PYTHONQTGENERATOR_EXECUTABLE not specified or inexistent when calling ctkMacroWrapPythonQt")
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

    # what is the filename without the extension
    get_filename_component(TMP_FILENAME ${FILE} NAME_WE)

    # Extract classname - NOTE: We assume the filename matches the associated class
    set(className ${TMP_FILENAME})

    if(NOT skip_wrapping)
      # Skip wrapping if IS_WRAP_FULL=FALSE and if file do NOT contain Q_OBJECT
      if(NOT IS_WRAP_FULL)
        file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} file_content)
        if(NOT "${file_content}" MATCHES "Q_OBJECT")
          set(skip_wrapping TRUE)
          ctkMacroWrapPythonQt_log("${FILE}: skipping - No Q_OBJECT macro")
        endif()
      endif()
    endif()

    if(NOT skip_wrapping)
      # Skip wrapping if IS_WRAP_FULL=FALSE and if constructor doesn't match:
      #    my_class()
      #    my_class(QObject* newParent ...)
      #    my_class(QWidget* newParent ...)
      if(NOT IS_WRAP_FULL)
        # Constructor with either QWidget or QObject as first parameter
        set(regex "[^~]${className}[\\s\\n]*\\([\\s\\n]*((QObject|QWidget)[\\s\\n]*\\*[\\s\\n]*\\w+[\\s\\n]*(\\=[\\s\\n]*(0|NULL)|,.*\\=.*\\)|\\)|\\)))")
        ctkMacroWrapPythonQt_reSearchfile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH}
                                          ${regex} ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} is_matching)
        if(NOT is_matching)
          set(skip_wrapping TRUE)
          ctkMacroWrapPythonQt_log("${FILE}: skipping - Missing expected constructor signature")
        endif()
      endif()
    endif()

    if(NOT skip_wrapping)
      # Skip wrapping if object has a virtual pure method
      # "x3b" is the unicode for semicolon
      set(regex "virtual[\\w\\n\\s\\*\\(\\)]+\\=[\\s\\n]*(0|NULL)[\\s\\n]*\\x3b")
      ctkMacroWrapPythonQt_reSearchfile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH}
                                        ${regex} ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} is_matching)
      if(is_matching)
        set(skip_wrapping TRUE)
        ctkMacroWrapPythonQt_log("${FILE}: skipping - Contains a virtual pure method")
      endif()
    endif()

    # if we should wrap it
    IF (NOT skip_wrapping)

      # the input file might be full path so handle that
      get_filename_component(TMP_FILEPATH ${FILE} PATH)

      # compute the input filename
      IF (TMP_FILEPATH)
        set(TMP_INPUT ${TMP_FILEPATH}/${TMP_FILENAME}.h)
      ELSE (TMP_FILEPATH)
        set(TMP_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.h)
      ENDIF (TMP_FILEPATH)

      list(APPEND SOURCES_TO_WRAP ${TMP_INPUT})

    endif()
  endforeach()

  # PythonQtGenerator expects a colon ':' separated list
  set(INCLUDE_DIRS_TO_WRAP)
  foreach(include ${CTK_BASE_INCLUDE_DIRS})
    set(INCLUDE_DIRS_TO_WRAP "${INCLUDE_DIRS_TO_WRAP}:${include}")
  endforeach()

  # Prepare custom_command argument
  set(SOURCES_TO_WRAP_ARG)
  foreach(source ${SOURCES_TO_WRAP})
    set(SOURCES_TO_WRAP_ARG "${SOURCES_TO_WRAP_ARG}^^${source}")
  endforeach()

  # Define wrap type and wrap intermediate directory
  set(wrap_type "Light")
  set(wrap_int_dir generated_cpp/${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}/)
  set(extra_files )
  if(${IS_WRAP_FULL})
    set(wrap_type "Full")
    set(extra_files ${wrap_int_dir}ctkPythonQt_${TARGET}_masterinclude.h)
  endif()
  #message("wrap_type:${wrap_type} - wrap_int_dir:${wrap_int_dir}")

  # Create intermediate output directory
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir})

  # On Windows, to avoid "too long input" error, dump INCLUDE_DIRS_TO_WRAP into a file
  if(WIN32)
    # File containing the moc flags
    set(include_dirs_to_wrap_filename includeDirsToWrap_${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}.txt)
    set(include_dirs_to_wrap_file ${CMAKE_CURRENT_BINARY_DIR}/${wrap_int_dir}${include_dirs_to_wrap_filename})
    file(WRITE ${include_dirs_to_wrap_file} ${INCLUDE_DIRS_TO_WRAP})
    # The arg passed to the custom command will be the file containing the list of include dirs to wrap
    set(INCLUDE_DIRS_TO_WRAP ${include_dirs_to_wrap_file})
  endif()

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
      ${extra_files}
    DEPENDS
      ${pythonqtgenerator_executable_depends}
      ${SOURCES_TO_WRAP}
      ${CTK_CMAKE_DIR}/ctkScriptWrapPythonQt_${wrap_type}.cmake
      ${CTK_CMAKE_DIR}/ctkMacroWrapPythonQtModuleInit.cpp.in
    COMMAND ${CMAKE_COMMAND}
      -DPYTHONQTGENERATOR_EXECUTABLE:FILEPATH=${PYTHONQTGENERATOR_EXECUTABLE}
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
      -DPYTHON_LIBRARY_PATH:PATH=${PYTHON_LIBRARY_PATH}
      -DWRAPPING_NAMESPACE:STRING=${WRAPPING_NAMESPACE}
      -DTARGET:STRING=${TARGET}
      -DSOURCES:STRING=${SOURCES_TO_WRAP_ARG}
      -DINCLUDE_DIRS:STRING=${INCLUDE_DIRS_TO_WRAP}
      -DOUTPUT_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
      -DWRAP_INT_DIR:STRING=${wrap_int_dir}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      -DHAS_DECORATOR:BOOL=${HAS_DECORATOR}
      -P ${CTK_CMAKE_DIR}/ctkScriptWrapPythonQt_${wrap_type}.cmake
    COMMENT "PythonQt ${wrap_type} Wrapping - Generating ${wrapper_init_cpp_filename}"
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
    COMMENT "PythonQt ${wrap_type} Wrapping - Moc'ing ${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET} wrapper headers"
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

