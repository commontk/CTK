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
#
#
FUNCTION(ctkFunctionLFtoCRLF input_file output_file)
  # Make sure the file exists
  IF(NOT EXISTS ${input_file})
    MESSAGE(FATAL_ERROR "Failed to convert LF to CRLF - File [${input_file}] does NOT exist !")
  ENDIF()

  # Read lines
  FILE(STRINGS "${input_file}" lines)

  SET(string_with_crlf)
  SET(first_line TRUE)
  # Loop over lines and append \r\n
  FOREACH(line ${lines})
    IF(first_line)
      SET(string_with_crlf ${line})
      SET(first_line FALSE)
    ELSE()
      SET(string_with_crlf "${string_with_crlf}\r\n${line}")
    ENDIF()
  ENDFOREACH()

  FILE(WRITE ${output_file} ${string_with_crlf})
ENDFUNCTION()

#
# Test - Use cmake -DFUNCTION_TESTING:BOOL=ON -DINPUT_FILE:FILEPATH=<FILE> -DOUTPUT_FILE:FILEPATH=<FILE> -P ctkFunctionLFtoCRLF.cmake
#
IF(FUNCTION_TESTING)
  IF(NOT DEFINED INPUT_FILE)
    MESSAGE(FATAL_ERROR "INPUT_FILE is not defined !")
  ENDIF()

  IF(NOT DEFINED OUTPUT_FILE)
    MESSAGE(FATAL_ERROR "OUTPUT_FILE is not defined !")
  ENDIF()

  #MESSAGE(STATUS "INPUT_FILE [${INPUT_FILE}]")
  #MESSAGE(STATUS "OUTPUT_FILE [${OUTPUT_FILE}]")
  
  ctkFunctionLFtoCRLF("${INPUT_FILE}" "${OUTPUT_FILE}")
ENDIF()