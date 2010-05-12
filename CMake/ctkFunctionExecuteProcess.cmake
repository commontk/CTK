###########################################################################
#
#  Library:   CTK
# 
#  Copyright (c) 2010  Kitware Inc.
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

FUNCTION(ctkFunctionExecuteProcess)
  CtkMacroParseArguments(MY
    "COMMAND;PATH_LIST;WORKING_DIRECTORY;TIMEOUT;RESULT_VARIABLE;OUTPUT_VARIABLE;ERROR_VARIABLE;INPUT_FILE;OUTPUT_FILE;ERROR_FILE"
    "OUTPUT_QUIET;ERROR_QUIET;OUTPUT_STRIP_TRAILING_WHITESPACE;ERROR_STRIP_TRAILING_WHITESPACE"
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_COMMAND)
    MESSAGE(SEND_ERROR "COMMAND is mandatory")
  ENDIF()

  # Extract executable
  LIST(LENGTH MY_COMMAND command_length)
  LIST(GET MY_COMMAND 0 MY_EXECUTABLE)
  GET_FILENAME_COMPONENT(EXECUTABLE_NAME ${MY_EXECUTABLE} NAME_WE)
  #MESSAGE("EXECUTABLE_NAME:${EXECUTABLE_NAME}")
  
  IF(NOT EXISTS ${MY_EXECUTABLE})
    MESSAGE(FATAL_ERROR "Exectuable ${MY_EXECUTABLE} doesn't exist !")
  ENDIF()
  
  # Command
  SET(CONFIG_COMMAND COMMAND)
  FOREACH(command_part ${MY_COMMAND})
    SET(CONFIG_COMMAND "${CONFIG_COMMAND} \"${command_part}\"")
  ENDFOREACH()
  #MESSAGE(STATUS CONFIG_COMMAND:"${CONFIG_COMMAND}")

  # Arguments list
  SET(arg_names WORKING_DIRECTORY TIMEOUT RESULT_VARIABLE OUTPUT_VARIABLE ERROR_VARIABLE INPUT_FILE OUTPUT_FILE ERROR_FILE)
  FOREACH(arg_name ${arg_names})
    IF(DEFINED MY_${arg_name})
      SET(CONFIG_${arg_name} "${arg_name} \"${MY_${arg_name}}\"")
    ENDIF()
  ENDFOREACH()

  # Options list
  SET(option_names OUTPUT_QUIET ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
  FOREACH(option_name ${option_names})
    IF(${MY_${option_name}})
      SET(CONFIG_${option_name} ${option_name})
    ENDIF()
  ENDFOREACH()

  SET(CONFIG_PATH_LIST ${MY_PATH_LIST})
  SET(VARNAME_RESULT_VARIABLE ${MY_RESULT_VARIABLE})
  SET(VARNAME_OUTPUT_VARIABLE ${MY_OUTPUT_VARIABLE})
  SET(VARNAME_ERROR_VARIABLE ${MY_ERROR_VARIABLE})

  SET(cmd ${CMAKE_CURRENT_BINARY_DIR}/CMake/ctkExecuteProcess-${EXECUTABLE_NAME}.cmake)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/ctkExecuteProcess.cmake.in
    ${cmd}
    @ONLY)

  # Execute process
  EXECUTE_PROCESS(
    COMMAND ${CMAKE_COMMAND} -P "${cmd}"
    RESULT_VARIABLE error_code
    )

  IF(error_code)
    MESSAGE(FATAL_ERROR "Failed to execute ${cmd}")
  ENDIF()
  

  SET(output_file "${CMAKE_CURRENT_BINARY_DIR}/CMake/ctkExecuteProcess-${EXECUTABLE_NAME}-output.cmake")
  IF(NOT EXISTS ${output_file})
    MESSAGE(FATAL_ERROR "Output file doesn't exist - Problem executing: ${cmd}")
  ENDIF()

  INCLUDE(${output_file})
  
ENDFUNCTION()