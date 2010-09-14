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
# Generate a DGrapgh input file expected by DGraph executable.
#
FUNCTION(ctkFunctionGenerateDGraphInput dir target_directories)
  IF(NOT EXISTS ${dir})
    MESSAGE(FATAL_ERROR "Directory ${dir} doesn't exist!")
  ENDIF()

  CtkMacroParseArguments(MY
    ""
    "WITH_OPTION;WITH_EXTERNALS"
    ${ARGN}
    )

  SET(dgraph_list )

  SET(edges)
  SET(vertices)
  
  FOREACH(target_info ${target_directories})

    # extract target_dir and option_name
    STRING(REPLACE "^^" "\\;" target_info ${target_info})
    SET(target_info_list ${target_info})
    LIST(GET target_info_list 0 target_dir)
    LIST(GET target_info_list 1 option_name)
    #MESSAGE(STATUS target_dir:${target_dir})
    #MESSAGE(STATUS option_name:${option_name})
    #MESSAGE(STATUS option_name-value:${${option_name}})
    
    # make sure the directory exists
    IF(NOT EXISTS ${target_dir}/CMakeLists.txt)
      MESSAGE(FATAL_ERROR "Target directory ${target_dir}/CMakeLists.txt doesn't exists !")
    ENDIF()

    SET(include_dep 1)
    IF(MY_WITH_OPTION)
      SET(include_dep ${${option_name}})
    ENDIF()
    IF(${include_dep})
      # extract project name from CMakeLists.txt
      FILE(STRINGS "${target_dir}/CMakeLists.txt" project_string
        REGEX "^ *(P|p)(R|r)(O|o)(J|j)(E|e)(C|c)(T|t)\\("
        LIMIT_COUNT 10)
      STRING(REGEX MATCH "\\((.*)\\)" target_project_name ${project_string})
      STRING(REGEX REPLACE "\\(|\\)" "" target_project_name ${target_project_name})
      IF(${target_project_name} STREQUAL "")
        MESSAGE(FATAL_ERROR "Failed to extract project name from ${target_dir}/CMakeLists.txt")
      ENDIF()
      #MESSAGE(STATUS target_project_name:${target_project_name})

      # Make sure the variable is cleared
      SET(dependencies )

      # get dependencies
      ctkMacroCollectTargetLibraryNames(${target_dir} dependencies)

      # Make sure the variable is cleared
      SET(ctk_dependencies)

      # filter dependencies starting with CTK
      ctkMacroGetAllCTKTargetLibraries("${dependencies}" ctk_dependencies)

      IF(ctk_dependencies)
        LIST(APPEND vertices ${target_project_name})
      ENDIF()

      # Generate XML related to the dependencies
      FOREACH(dependency_name ${ctk_dependencies})
        LIST(APPEND edges ${dependency_name})
        SET(dgraph_list ${dgraph_list} "${target_project_name} ${dependency_name}\n")
        LIST(APPEND vertices ${dependency_name})
      ENDFOREACH()

    ENDIF()
    
  ENDFOREACH()

  IF(vertices)
    LIST(REMOVE_DUPLICATES vertices)
  ENDIF()
  LIST(LENGTH vertices numberOfVertices)
  LIST(LENGTH edges numberOfEdges)

  SET(dgraph_list "${numberOfVertices} ${numberOfEdges}\n" ${dgraph_list})

  IF(${with_option})
    SET(filename "${dir}/DGraphInput.txt")
  ELSE()
    SET(filename "${dir}/DGraphInput-alldep.txt")
  ENDIF()

  FILE(WRITE ${filename} ${dgraph_list})
  MESSAGE(STATUS "Generated: ${filename}")
ENDFUNCTION()
