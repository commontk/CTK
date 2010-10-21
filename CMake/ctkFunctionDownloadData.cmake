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
# Fetch data from a MIDAS server
#
# Usage:
#  ctkFunctionDownloadData(
#         MIDAS_SERVER_URL http://www.insight-journal.org/midas/item/download/
#         ITEMID 2461
#         OUTPUT_DIRECTORY /home/jchris/Projects/Data
#         [FORCE_DOWNLOAD]
#         )
#
# The downloaded file will have the from: midas_item_<ITEMID>.tar
#
FUNCTION( ctkFunctionDownloadData)
  ctkMacroParseArguments(MY
    "MIDAS_SERVER_URL;ITEMID;OUTPUT_DIRECTORY"
    "FORCE_DOWNLOAD"
    ${ARGN}
    )

  # Sanity checks
  FOREACH(arg MIDAS_SERVER_URL ITEMID OUTPUT_DIRECTORY)
    IF(NOT DEFINED MY_${arg})
      MESSAGE(SEND_ERROR "${arg} is mandatory")
    ENDIF()
  ENDFOREACH()
    
  # Make sure output directory exists
  IF(NOT EXISTS "${MY_OUTPUT_DIRECTORY}")
    MESSAGE(FATAL_ERROR "OUTPUT_DIRECTORY '${MY_OUTPUT_DIRECTORY}' doesn't exist !")
  ENDIF()

  # Is download required ?
  SET(dest_file ${MY_OUTPUT_DIRECTORY}/midas_item_${MY_ITEMID}.tar)
  IF (NOT EXISTS ${dest_file} OR MY_FORCE_DOWNLOAD)
    SET(url ${MY_MIDAS_SERVER_URL}/${MY_ITEMID})
    FILE(DOWNLOAD ${url} ${dest_file} STATUS status)
    LIST(GET status 0 error_code)
    LIST(GET status 1 error_msg)
    IF(error_code)
      MESSAGE(FATAL_ERROR "error: Failed to download ${url} - ${error_msg}")
    ENDIF()
    MESSAGE(STATUS "info: downloaded '${dest_file}'")
  ENDIF()
  
ENDFUNCTION()

#
# Test - Use cmake -DMACRO_TESTING:BOOL=ON -P ctkFunctionDownloadData.cmake
#
IF(FUNCTION_TESTING)

  INCLUDE(ctkMacroParseArguments.cmake)

  MESSAGE("Testing ctkFunctionDownloadData ...")
  
  #
  # Test1
  #
  SET(url http://www.insight-journal.org/midas/item/download/)
  SET(output_dir $ENV{HOME}/Projects/Data)
  SET(itemid 2461)
  MESSAGE(STATUS "downloading... [http://www.insight-journal.org/midas/item/download/${itemid}]")
  ctkFunctionDownloadData(
    MIDAS_SERVER_URL ${url}
    ITEMID ${itemid}
    OUTPUT_DIRECTORY ${output_dir}
    )

  SET(expected_file ${output_dir}/midas_item_${itemid}.tar)
  # Make sure the file exists
  IF(NOT EXISTS ${expected_file})
    MESSAGE(FATAL_ERROR "File '${expected_file}' doesn't exists")
  ENDIF()

  SET(extract_destination_dir ${output_dir}/item_${itemid})
  
  # Create a folder
  MESSAGE(STATUS "creating directory ... [${extract_destination_dir}]")
  FILE(MAKE_DIRECTORY ${extract_destination_dir})

  # Extract
  SET(tar_args xf)
  MESSAGE(STATUS "extracting... [tar midas_item_${itemid}.tar]")
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar ${tar_args} ${expected_file}
    WORKING_DIRECTORY ${extract_destination_dir}
    RESULT_VARIABLE rv)

  IF(NOT rv EQUAL 0)
    MESSAGE(STATUS "extracting... [error clean up]")
    FILE(REMOVE_RECURSE ${extract_destination_dir})
    MESSAGE(FATAL_ERROR "error: extract of '${expected_file}' failed")
  ENDIF()

  # Remove archive
  #file(REMOVE ${expected_file})
  #file(REMOVE_RECURSE ${extract_destination_dir})
ENDIF()
