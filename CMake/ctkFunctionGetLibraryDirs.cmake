###########################################################################
#
#  Library: CTK
#
#  Copyright (c) German Cancer Research Center,
#    Division of Medical and Biological Informatics
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#! \ingroup CMakeUtilities
function(ctkFunctionGetLibraryDirs var_library_dirs)

  if(NOT ARGN)
    message(FATAL_ERROR "No targets given")
  endif()

  set(_library_dirs ${${var_library_dirs}})
  foreach(_target ${ARGN})

    # Add the library directories from the external project
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro ctkMacroValidateBuildOptions

    set(ext_deps )

    ctkMacroGetAllNonProjectTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)
    
    foreach(dep ${ext_deps})

      if(${dep}_LIBRARY_DIRS)
        string(REPLACE "^" ";" _ext_library_dirs "${${dep}_LIBRARY_DIRS}")
        list(APPEND _library_dirs ${_ext_library_dirs})
      endif()

    endforeach()

  endforeach()

  if(_library_dirs)
    list(REMOVE_DUPLICATES _library_dirs)
  endif()
  set(${var_library_dirs} ${_library_dirs} PARENT_SCOPE)

endfunction()
