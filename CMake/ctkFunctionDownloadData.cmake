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


#!
#! Fetch data from a MIDAS server
#!
#! Usage:
#! \code
#!  ctkFunctionDownloadData(
#!         MIDAS_SERVER_URL http://www.insight-journal.org/midas/item/download/
#!         ITEMID 2461
#!         OUTPUT_DIRECTORY /home/jchris/Projects/Data
#!         [FORCE_DOWNLOAD]
#!         )
#! \endcode
#!
#! The downloaded file will have the from: midas_item_<ITEMID>.tar
#!
#! \ingroup CMakeUtilities
function( ctkFunctionDownloadData)
  ctkMacroParseArguments(MY
    "MIDAS_SERVER_URL;ITEMID;OUTPUT_DIRECTORY"
    "FORCE_DOWNLOAD"
    ${ARGN}
    )

  # Sanity checks
  foreach(arg MIDAS_SERVER_URL ITEMID OUTPUT_DIRECTORY)
    if(NOT DEFINED MY_${arg})
      message(FATAL_ERROR "${arg} is mandatory")
    endif()
  endforeach()
    
  # Make sure output directory exists
  if(NOT EXISTS "${MY_OUTPUT_DIRECTORY}")
    message(FATAL_ERROR "OUTPUT_DIRECTORY '${MY_OUTPUT_DIRECTORY}' doesn't exist !")
  endif()

  # Is download required ?
  set(dest_file ${MY_OUTPUT_DIRECTORY}/midas_item_${MY_ITEMID}.tar)
  IF (NOT EXISTS ${dest_file} OR MY_FORCE_DOWNLOAD)
    set(url ${MY_MIDAS_SERVER_URL}/${MY_ITEMID})
    file(DOWNLOAD ${url} ${dest_file} STATUS status)
    list(GET status 0 error_code)
    list(GET status 1 error_msg)
    if(error_code)
      message(FATAL_ERROR "error: Failed to download ${url} - ${error_msg}")
    endif()
    message(STATUS "info: downloaded '${dest_file}'")
  endif()
  
endfunction()

#
# Test - Use cmake -DMACRO_TESTING:BOOL=ON -P ctkFunctionDownloadData.cmake
#
if(FUNCTION_TESTING)

  include(ctkMacroParseArguments.cmake)

  message("Testing ctkFunctionDownloadData ...")
  
  #
  # Test1
  #
  set(url http://www.insight-journal.org/midas/item/download/)
  set(output_dir $ENV{HOME}/Projects/Data)
  set(itemid 2461)
  message(STATUS "downloading... [http://www.insight-journal.org/midas/item/download/${itemid}]")
  ctkFunctionDownloadData(
    MIDAS_SERVER_URL ${url}
    ITEMID ${itemid}
    OUTPUT_DIRECTORY ${output_dir}
    )

  set(expected_file ${output_dir}/midas_item_${itemid}.tar)
  # Make sure the file exists
  if(NOT EXISTS ${expected_file})
    message(FATAL_ERROR "File '${expected_file}' doesn't exists")
  endif()

  set(extract_destination_dir ${output_dir}/item_${itemid})
  
  # Create a folder
  message(STATUS "creating directory ... [${extract_destination_dir}]")
  file(MAKE_DIRECTORY ${extract_destination_dir})

  # Extract
  set(tar_args xf)
  message(STATUS "extracting... [tar midas_item_${itemid}.tar]")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar ${tar_args} ${expected_file}
    WORKING_DIRECTORY ${extract_destination_dir}
    RESULT_VARIABLE rv)

  if(NOT rv EQUAL 0)
    message(STATUS "extracting... [error clean up]")
    file(REMOVE_RECURSE ${extract_destination_dir})
    message(FATAL_ERROR "error: extract of '${expected_file}' failed")
  endif()

  # Remove archive
  #file(REMOVE ${expected_file})
  #file(REMOVE_RECURSE ${extract_destination_dir})
endif()
