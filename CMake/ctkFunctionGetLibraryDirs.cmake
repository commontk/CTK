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

FUNCTION(ctkFunctionGetLibraryDirs var_library_dirs)

  IF(NOT ARGN)
    MESSAGE(FATAL_ERROR "No targets given")
  ENDIF()

  SET(_library_dirs ${${var_library_dirs}})
  FOREACH(_target ${ARGN})

    # Add the library directories from the external project
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro ctkMacroValidateBuildOptions

    SET(ext_deps )

    ctkMacroGetAllNonCTKTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)
    
    FOREACH(dep ${ext_deps})

      IF(${dep}_LIBRARY_DIRS)
        STRING(REPLACE "^" ";" _ext_library_dirs "${${dep}_LIBRARY_DIRS}")
        LIST(APPEND _library_dirs ${_ext_library_dirs})
      ENDIF()

    ENDFOREACH()

  ENDFOREACH()

  IF(_library_dirs)
    LIST(REMOVE_DUPLICATES _library_dirs)
  ENDIF()
  SET(${var_library_dirs} ${_library_dirs} PARENT_SCOPE)

ENDFUNCTION()
