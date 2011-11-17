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

#! \ingroup CMakeUtilities
function(ctkFunctionLFtoCRLF input_file output_file)
  # Make sure the file exists
  if(NOT EXISTS ${input_file})
    message(FATAL_ERROR "Failed to convert LF to CRLF - File [${input_file}] does NOT exist !")
  endif()

  # Read lines
  file(STRINGS "${input_file}" lines)

  set(string_with_crlf)
  set(first_line TRUE)
  # Loop over lines and append \r\n
  foreach(line ${lines})
    if(first_line)
      set(string_with_crlf ${line})
      set(first_line FALSE)
    else()
      set(string_with_crlf "${string_with_crlf}\r\n${line}")
    endif()
  endforeach()

  file(WRITE ${output_file} ${string_with_crlf})
endfunction()

#
# Test - Use cmake -DFUNCTION_TESTING:BOOL=ON -DINPUT_FILE:FILEPATH=<FILE> -DOUTPUT_FILE:FILEPATH=<FILE> -P ctkFunctionLFtoCRLF.cmake
#
if(FUNCTION_TESTING)
  if(NOT DEFINED INPUT_FILE)
    message(FATAL_ERROR "INPUT_FILE is not defined !")
  endif()

  if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE is not defined !")
  endif()

  #message(STATUS "INPUT_FILE [${INPUT_FILE}]")
  #message(STATUS "OUTPUT_FILE [${OUTPUT_FILE}]")
  
  ctkFunctionLFtoCRLF("${INPUT_FILE}" "${OUTPUT_FILE}")
endif()
