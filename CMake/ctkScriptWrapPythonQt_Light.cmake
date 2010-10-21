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
#      http://www.commontk.org/LICENSE
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
FUNCTION(log msg)
  IF(verbose)
    MESSAGE(${msg})
  ENDIF()
  FILE(APPEND "${CMAKE_CURRENT_BINARY_DIR}/ctkScriptWrapPythonQt_Light_log.txt" "${msg}\n")
ENDFUNCTION()

#
# Convenient function allowing to invoke re.search(regex, string) using the given interpreter.
# Note that is_matching will be set to True if there is a match
#
FUNCTION(reSearchFile python_exe python_library_path regex file is_matching)

  set(python_cmd "import re\; f = open('${file}', 'r')\;
res = re.search\(\"${regex}\", f.read(), re.MULTILINE\)\;
if res == None: print \"FALSE\" 
else: print \"TRUE\"
")
  #message("python_cmd: ${python_cmd}")
  
  IF(WIN32)
    SET(ENV{PATH} ${python_library_path};$ENV{PATH})
  ELSEIF(APPLE)
    SET(ENV{DYLD_LIBRARY_PATH} ${python_library_path}:$ENV{DYLD_LIBRARY_PATH})
  ELSE()
    SET(ENV{LD_LIBRARY_PATH} ${python_library_path}:$ENV{LD_LIBRARY_PATH})
  ENDIF()

  EXECUTE_PROCESS(
    COMMAND ${python_exe} -c ${python_cmd};
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  
  IF(result)
    MESSAGE(SEND_ERROR "reSearchFile - Problem with regex: ${regex}\n${error}")
  ENDIF()
  #message(${output})
  SET(is_matching ${output} PARENT_SCOPE)
  
ENDFUNCTION()


# Check for non-defined var
FOREACH(var WRAPPING_NAMESPACE TARGET SOURCES INCLUDE_DIRS WRAP_INT_DIR)
  IF(NOT DEFINED ${var})
    MESSAGE(SEND_ERROR "${var} not specified when calling ctkScriptWrapPythonQt")
  ENDIF()
ENDFOREACH()

# Check for non-existing ${var}
FOREACH(var QT_QMAKE_EXECUTABLE OUTPUT_DIR PYTHON_EXECUTABLE PYTHON_LIBRARY_PATH)
  IF(NOT EXISTS ${${var}})
    MESSAGE(SEND_ERROR "Failed to find ${var} when calling ctkScriptWrapPythonQt")
  ENDIF()
ENDFOREACH()

# Clear log file
FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ctkScriptWrapPythonQt_Light_log.txt" "")
  
# Convert wrapping namespace to subdir
STRING(REPLACE "." "_" WRAPPING_NAMESPACE_UNDERSCORE ${WRAPPING_NAMESPACE})

# Convert ^^ separated string to list
STRING(REPLACE "^^" ";" SOURCES "${SOURCES}")

FOREACH(FILE ${SOURCES})

  # what is the filename without the extension
  GET_FILENAME_COMPONENT(TMP_FILENAME ${FILE} NAME_WE)
      
  SET(includes 
    "${includes}\n#include \"${TMP_FILENAME}.h\"")
        
  # Extract classname - NOTE: We assume the filename matches the associated class
  set(className ${TMP_FILENAME})
  #message(STATUS "FILE:${FILE}, className:${className}")
  
  # Extract parent classname
  SET(parentClassName)
  
  IF("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass()
    SET(regex "[^~]${className}[\\s\\n]*\\([\\s\\n]*\\)")
    reSearchFile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    IF(is_matching)
      SET(parentClassName "No")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(\)")
    ENDIF()
  ENDIF()
  
  IF("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass(QObject * parent ...)
    SET(regex "${className}[\\s\\n]*\\([\\s\\n]*QObject[\\s\\n]*\\*[\\s\\n]*\\w+[\\s\\n]*(\\=[\\s\\n]*(0|NULL)|,.*\\=.*\\)|\\))")
    reSearchFile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    IF(is_matching)
      SET(parentClassName "QObject")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(QObject * parent ... \)")
    ENDIF()
  ENDIF()
  
  IF("${parentClassName}" STREQUAL "")
    # Does constructor signature is of the form: myclass(QWidget * parent ...)
    SET(regex "${className}[\\s\\n]*\\([\\s\\n]*QWidget[\\s\\n]*\\*[\\s\\n]*\\w+[\\s\\n]*(\\=[\\s\\n]*(0|NULL)|,.*\\=.*\\)|\\))")
    reSearchFile(${PYTHON_EXECUTABLE} ${PYTHON_LIBRARY_PATH} ${regex} ${FILE} is_matching)
    IF(is_matching)
      SET(parentClassName "QWidget")
      log("${TMP_FILENAME} - constructor of the form: ${className}\(QWidget * parent ... \)")
    ENDIF()
  ENDIF()
 
  # Generate PythonQtWrapper class
  IF("${parentClassName}" STREQUAL "QObject" OR "${parentClassName}" STREQUAL "QWidget")
  
    SET(pythonqtWrappers 
      "${pythonqtWrappers}
//-----------------------------------------------------------------------------
class PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public slots:
  ${className}* new_${className}(${parentClassName}*  parent = 0)
    {
    return new ${className}(parent);
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
")

  ELSEIF("${parentClassName}" STREQUAL "No")
  
    SET(pythonqtWrappers 
      "${pythonqtWrappers}
//-----------------------------------------------------------------------------
class PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public slots:
  ${className}* new_${className}()
    {
    return new ${className}();
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
")

  ELSE() # Case parentClassName is empty
  
    MESSAGE(WARNING "ctkScriptWrapPythonQt_Light - Problem wrapping ${FILE}")
    
  ENDIF()

  # Generate code allowing to register the class metaobject and its associated "light" wrapper
  SET(registerclasses "${registerclasses}
  PythonQt::self()->registerClass(
    &${className}::staticMetaObject, \"${TARGET}\",   
    PythonQtCreateObject<PythonQtWrapper_${className}>);\n")

ENDFOREACH()

# Write master include file
FILE(WRITE ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}0.h "//
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
FILE(WRITE ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp "//
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

# Since FILE(WRITE ) doesn't update the timestamp - Let's touch the files
EXECUTE_PROCESS(
  COMMAND ${CMAKE_COMMAND} -E touch 
    ${OUTPUT_DIR}/${WRAP_INT_DIR}${WRAPPING_NAMESPACE_UNDERSCORE}_${TARGET}_init.cpp
  )

