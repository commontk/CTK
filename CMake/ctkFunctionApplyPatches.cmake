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
# Depends on:
#  CTK/CMake/ctkFunctionLFtoCRLF.cmake
#

#! \ingroup CMakeUtilities
function(ctkFunctionApplyPatches PATCH_EXE SRC_DIR PATCH_FILES)
  
  # Make sure patch executable exists
  if(NOT EXISTS ${PATCH_EXE})
    message(FATAL_ERROR "ERROR: Program [${PATCH_EXE}] doesn't exist !")
  endif()

  # Apply patches
  foreach(patch_file ${PATCH_FILES})
    message("Applying patch: ${patch_file}")
    if(WIN32)
      ctkFunctionLFtoCRLF("${patch_file}" "${patch_file}")
    endif()
    execute_process(COMMAND ${PATCH_EXE} -p0 -i "${patch_file}" -d "${SRC_DIR}" RESULT_VARIABLE result_var)
    if(result_var)
      message("ERROR: ${result_var}")
    endif()
  endforeach()
  
endfunction()
