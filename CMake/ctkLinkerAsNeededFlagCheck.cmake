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
# Linker --as-needed flag check
#
# Check if the linker will resolve symbols of underlinked libraries
#
# This script set the variable CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED
# to either TRUE or FALSE.
#

if(NOT DEFINED CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED)
  message(STATUS "Checking if --no-as-needed linker flag is required")
  set(LINK_TEST_SOURCE_DIR ${CTK_CMAKE_DIR}/ctkLinkerAsNeededFlagCheck)
  set(LINK_TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ctkLinkerAsNeededFlagCheck)
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${LINK_TEST_BINARY_DIR})
  try_compile(CTK_LINKER_LINKS_UNDERLINKED_LIBS
    ${LINK_TEST_BINARY_DIR}
    ${LINK_TEST_SOURCE_DIR}
    LINK_TEST
    )
  if(CTK_LINKER_LINKS_UNDERLINKED_LIBS)
    set(CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED FALSE CACHE INTERNAL "Test CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED")
    message(STATUS "Checking if --no-as-needed linker flag is required - no")
  else()
    try_compile(CTK_LINKER_NO_AS_NEEDED_LINKS_UNDERLINKED_LIBS
      ${LINK_TEST_BINARY_DIR}
      ${LINK_TEST_SOURCE_DIR}
      LINK_TEST_FLAGS
      CMAKE_FLAGS -DCMAKE_EXE_LINKER_FLAGS=-Wl,--no-as-needed
      )
    if(CTK_LINKER_NO_AS_NEEDED_LINKS_UNDERLINKED_LIBS)
      set(CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED TRUE CACHE INTERNAL "Test CTK_LINKER_NO_AS_NEEDED_FLAG_REQUIRED")
      message(STATUS "Checking if --no-as-needed linker flag is required - yes")
    else()
      message(STATUS "Checking if --no-as-needed linker flag is required - failed.")
      message(WARNING "Could not compile test code."
        "Linker could fail trying to resolve symbols for underlinked libraries."
        "See issue 2321 (http://na-mic.org/Mantis/view.php?id=2321) for more details.")
    endif()
  endif()
endif()
