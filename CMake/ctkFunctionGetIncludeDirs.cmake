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

FUNCTION(ctkFunctionGetIncludeDirs var_include_dirs)

  IF(NOT ARGN)
    MESSAGE(FATAL_ERROR "No targets given")
  ENDIF()

  SET(_include_dirs ${${var_include_dirs}})
  FOREACH(_target ${ARGN})

    # Add the include directories from the plugin dependencies
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro ctkMacroValidateBuildOptions

    SET(ctk_deps )
    SET(ext_deps )

    ctkMacroGetAllCTKTargetLibraries("${${_target}_DEPENDENCIES}" ctk_deps)
    ctkMacroGetAllNonCTKTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)

    FOREACH(dep ${ctk_deps})
      LIST(APPEND _include_dirs
           ${${dep}_SOURCE_DIR}
           ${${dep}_BINARY_DIR}
           )

      # For external projects, CTKConfig.cmake contains variables
      # listening the include dirs for CTK libraries and plugins
      IF(${dep}_INCLUDE_DIRS)
        LIST(APPEND _include_dirs ${${dep}_INCLUDE_DIRS})
      ENDIF()
    ENDFOREACH()

    FOREACH(dep ${ext_deps})

      IF(${dep}_INCLUDE_DIRS)
        STRING(REPLACE "^" ";" _ext_include_dirs "${${dep}_INCLUDE_DIRS}")
        LIST(APPEND _include_dirs ${_ext_include_dirs})
      ENDIF()

      # This is for resolving include dependencies between
      # libraries / plugins from external projects using CTK
      IF(${dep}_SOURCE_DIR)
        LIST(APPEND _include_dirs ${${dep}_SOURCE_DIR})
      ENDIF()

      IF(${dep}_BINARY_DIR)
        LIST(APPEND _include_dirs ${${dep}_BINARY_DIR})
      ENDIF()

    ENDFOREACH()

  ENDFOREACH()

  IF(_include_dirs)
    LIST(REMOVE_DUPLICATES _include_dirs)
  ENDIF()
  SET(${var_include_dirs} ${_include_dirs} PARENT_SCOPE)

ENDFUNCTION()
