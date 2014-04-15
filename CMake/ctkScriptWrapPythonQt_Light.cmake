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
# This script should be invoked either as a CUSTOM_COMMAND
# or from the command line using the following syntax:
#
#    cmake -DWRAPPING_SCRIPT:FILEPATH=/path/to/ctkWrapPythonQt.py
#          -DWRAPPING_NAMESPACE:STRING=org.commontk -DTARGET:STRING=MyLib
#          -DSOURCES:STRING="file1^^file2"
#          -DOUTPUT_DIR:PATH=/path
#          -DPYTHON_EXECUTABLE:FILEPATH=/path/to/python
#          -DPYTHON_LIBRARY_PATH:PATH=/path/to/pythonlib
#          -P ctkScriptWrapPythonQt_Light.cmake
#

#
# LOG FILE:
#   File ctkScriptWrapPythonQt_Light_log.txt will be created in the current directory.
#   It will contain the list of class and the constructor signature that will be wrapped.
#

# Check for non-defined var
foreach(var WRAPPING_NAMESPACE TARGET SOURCES)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "${var} not specified when calling ctkScriptWrapPythonQt")
  endif()
endforeach()

# Check for non-existing ${var}
foreach(var WRAPPING_SCRIPT OUTPUT_DIR PYTHON_EXECUTABLE PYTHON_LIBRARY_PATH)
  if(NOT EXISTS ${${var}})
    message(FATAL_ERROR "Failed to find ${var}=\"${${var}}\" when calling ctkScriptWrapPythonQt")
  endif()
endforeach()

# Convert ^^ separated string to list
string(REPLACE "^^" ";" SOURCES "${SOURCES}")

if(WIN32)
  set(ENV{PATH} ${PYTHON_LIBRARY_PATH};$ENV{PATH})
elseif(APPLE)
  set(ENV{DYLD_LIBRARY_PATH} ${PYTHON_LIBRARY_PATH}:$ENV{DYLD_LIBRARY_PATH})
else()
  set(ENV{LD_LIBRARY_PATH} ${PYTHON_LIBRARY_PATH}:$ENV{LD_LIBRARY_PATH})
endif()

set(verbose 0)

set(extra_args)
if(verbose)
  set(extra_args --extra-verbose)
endif()

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} ${WRAPPING_SCRIPT}
    --target=${TARGET} --namespace=${WRAPPING_NAMESPACE} --output-dir=${OUTPUT_DIR} ${extra_args}
    ${SOURCES}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Failed to wrap target: ${TARGET}\n${output}\n${error}")
else()
  if(verbose)
    message(${output})
  endif()
endif()


