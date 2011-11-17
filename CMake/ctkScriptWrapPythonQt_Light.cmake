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
# ctkScriptWrapPythonQt_Light
#

#
# Depends on:
#  CTK/CMake/ctkMacroWrapPythonQt.cmake
#

#
# This script should be invoked either as a CUSTOM_COMMAND
# or from the command line using the following syntax:
#
#    cmake -DWRAPPING_NAMESPACE:STRING=org.commontk -DTARGET:STRING=MyLib
#          -DSOURCES:STRING="file1^^file2" -DINCLUDE_DIRS:STRING=/path1:/path2
#          -DWRAP_INT_DIR:STRING=subir/subir/
#          -DOUTPUT_DIR:PATH=/path  -DQT_QMAKE_EXECUTABLE:PATH=/path/to/qt/qmake
#          -DPYTHON_EXECUTABLE:FILEPATH=/path/to/python
#          -DPYTHON_LIBRARY_PATH:PATH=/path/to/pythonlib
#          -DHAS_DECORATOR:BOOL=True
#          -P ctkScriptWrapPythonQt_Light.cmake
#

#
# LOG FILE:
#   File ctkScriptWrapPythonQt_Light_log.txt will be created in the current directory.
#   It will contain the list of class and the constructor signature that will be wrapped.
#

set(verbose 0)

#
# Convenient function allowing to log the reason why a given class hasn't been wrapped
# If verbose=1, it will also be displayed on the standard output
#
function(log msg)
  if(verbose)
    message(${msg})
  endif()
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/ctkScriptWrapPythonQt_Light_log.txt" "${msg}\n")
endfunction()

#
# Convenient function allowing to invoke re.search(regex, string) using the given interpreter.
# Note that is_matching will be set to True if there is a match
#
function(reSearchFile python_exe python_library_path regex file is_matching)

  set(python_cmd "import re\; f = open('${file}', 'r')\;
res = re.search\(\"${regex}\", f.read(), re.MULTILINE\)\;
if res == None: print \"FALSE\"
else: print \"TRUE\"
")
  #message("python_cmd: ${python_cmd}")

  if(WIN32)
    set(ENV{PATH} ${python_library_path};$ENV{PATH})
  elseif(APPLE)
    set(ENV{DYLD_LIBRARY_PATH} ${python_library_path}:$ENV{DYLD_LIBRARY_PATH})
  else()
    set(ENV{LD_LIBRARY_PATH} ${python_library_path}:$ENV{LD_LIBRARY_PATH})
  endif()

  execute_process(
    COMMAND ${python_exe} -c ${python_cmd};
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  if(result)
    message(FATAL_ERROR "reSearchFile - Problem with regex: ${regex}\n${error}")
  endif()
  #message(${output})
  set(is_matching ${output} PARENT_SCOPE)

endfunction()

if(NOT DEFINED CMAKE_CURRENT_LIST_DIR)
  get_filename_component(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
endif()
if(NOT DEFINED CMAKE_CURRENT_LIST_FILENAME)
  get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME)
endif()

# Check for non-defined var
foreach(var WRAPPING_NAMESPACE TARGET SOURCES INCLUDE_DIRS WRAP_INT_DIR HAS_DECORATOR)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "${var} not specified when calling ctkScriptWrapPythonQt")
  endif()
endforeach()

# Check for non-existing ${var}
foreach(var QT_QMAKE_EXECUTABLE OUTPUT_DIR PYTHON_EXECUTABLE PYTHON_LIBRARY_PATH)
  if(NOT EXISTS ${${var}})
    message(FATAL_ERROR "Failed to find ${var}=\"${${var}}\" when calling ctkScriptWrapPythonQt")
  endif()
endforeach()

# Clear log file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ctkScriptWrapPythonQt_Light_log.txt" "")

# Convert wrapping namespace to subdir
string(REPLACE "." "_" WRAPPING_NAMESPACE_UNDERSCORE ${WRAPPING_NAMESPACE})

# Convert ^^ separated string to list
string(REPLACE "^^" ";" SOURCES "${SOURCES}")

foreach(FILE ${SOURCES})

  # what is the filename without the extension
  get_filename_component(TMP_FILENAME ${FILE} NAME_WE)

  set(includes
    "${includes}\n#include \"${TMP_FILENAME}.h\"")

  # Extract classname - NOTE: We assume the filename matches the associated class
  set(className ${TMP_FILENAME})
  #message(STATUS "FILE:${FILE}, className:${className}")

  # Extract parent classname
  set(parentClassName)

  if("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass()
    set(regex "[^~]${className}[\\s\\n]*\\([\\s\\n]*\\)")
    reSearchfile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    if(is_matching)
      set(parentClassName "No")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(\)")
    endif()
  endif()

  if("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass(QObject * parent ...)
    set(regex "${className}[\\s\\n]*\\([\\s\\n]*QObject[\\s\\n]*\\*[\\s\\n]*\\w+[\\s\\n]*(\\=[\\s\\n]*(0|NULL)|,.*\\=.*\\)|\\))")
    reSearchfile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    if(is_matching)
      set(parentClassName "QObject")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(QObject * parent ... \)")
    endif()
  endif()

  if("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass(QWidget * parent ...)
    set(regex "${className}[\\s\\n]*\\([\\s\\n]*QWidget[\\s\\n]*\\*[\\s\\n]*\\w+[\\s\\n]*(\\=[\\s\\n]*(0|NULL)|,.*\\=.*\\)|\\))")
    reSearchfile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    if(is_matching)
      set(parentClassName "QWidget")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(QWidget * parent ... \)")
    endif()
  endif()

  # Generate PythonQtWrapper class
  if("${parentClassName}" STREQUAL "QObject" OR "${parentClassName}" STREQUAL "QWidget")

    set(pythonqtWrappers
      "${pythonqtWrappers}
//-----------------------------------------------------------------------------
class PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public Q_SLOTS:
  ${className}* new_${className}(${parentClassName}*  parent = 0)
    {
    return new ${className}(parent);
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
")

  elseif("${parentClassName}" STREQUAL "No")

    set(pythonqtWrappers
      "${pythonqtWrappers}
//-----------------------------------------------------------------------------
class Q_DECL_EXPORT PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public Q_SLOTS:
  ${className}* new_${className}()
    {
    return new ${className}();
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
")

  else() # Case parentClassName is empty

    message(WARNING "ctkScriptWrapPythonQt_Light - Problem wrapping ${FILE}")

  endif()

  # Generate code allowing to register the class metaobject and its associated "light" wrapper
  set(registerclasses "${registerclasses}
  PythonQt::self()->registerClass(
    &${className}::staticMetaObject, \"${TARGET}\",
    PythonQtCreateObject<PythonQtWrapper_${className}>);\n")

endforeach()

# Write master include file
file(WRITE ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}0.h "//
// File auto-generated by cmake macro ctkScriptWrapPythonQt_Light
//

#ifndef __${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}0_h
#define __${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}0_h

#include <QWidget>
${includes}
${pythonqtWrappers}
#endif
")

# Write wrapper header
file(WRITE ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp "//
// File auto-generated by cmake macro ctkScriptWrapPythonQt_Light
//

#include <PythonQt.h>
#include \"${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}0.h\"

void PythonQt_init_${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}(PyObject* module)
{
  Q_UNUSED(module);
  ${registerclasses}
}
")

# Configure 'ctkMacroWrapPythonQtModuleInit.cpp.in' replacing TARGET and
# WRAPPING_NAMESPACE_UNDERSCORE.
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/ctkMacroWrapPythonQtModuleInit.cpp.in
  ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_module_init.cpp
  )

# Since file(WRITE ) doesn't update the timestamp - Let's touch the files
execute_process(
  COMMAND ${CMAKE_COMMAND} -E touch
    ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp
  )

