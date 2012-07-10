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
function(ctkFunctionGetIncludeDirs var_include_dirs)

  if(NOT ARGN)
    message(FATAL_ERROR "No targets given")
  endif()

  set(_include_dirs ${${var_include_dirs}} ${CTK_CONFIG_H_INCLUDE_DIR})
  foreach(_target ${ARGN})

    # Add the include directories from the plugin dependencies
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro ctkMacroValidateBuildOptions

    set(ctk_deps )
    set(ext_deps )

    ctkMacroGetAllProjectTargetLibraries("${${_target}_DEPENDENCIES}" ctk_deps)
    ctkMacroGetAllNonProjectTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)

    foreach(dep ${ctk_deps})

      if(${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      else()
        list(APPEND _include_dirs
             ${${dep}_SOURCE_DIR}
             ${${dep}_BINARY_DIR}
             )
      endif()

      # For external projects, CTKConfig.cmake contains variables
      # listening the include dirs for CTK libraries and plugins
      if(${dep}_INCLUDE_DIRS)
        list(APPEND _include_dirs ${${dep}_INCLUDE_DIRS})
      endif()
    endforeach()

    foreach(dep ${ext_deps})

      if(${dep}_INCLUDE_DIRS)
        string(REPLACE "^" ";" _ext_include_dirs "${${dep}_INCLUDE_DIRS}")
        list(APPEND _include_dirs ${_ext_include_dirs})
      endif()

      # This is for resolving include dependencies between
      # libraries / plugins from external projects using CTK
      if(${dep}_SOURCE_DIR AND ${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      elseif(${dep}_SOURCE_DIR)
        list(APPEND _include_dirs ${${dep}_SOURCE_DIR})
      endif()

      if(${dep}_BINARY_DIR)
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      endif()

    endforeach()

  endforeach()

  if(_include_dirs)
    list(REMOVE_DUPLICATES _include_dirs)
  endif()
  set(${var_include_dirs} ${_include_dirs} PARENT_SCOPE)

endfunction()
