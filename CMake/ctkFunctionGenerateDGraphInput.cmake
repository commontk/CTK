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
  SET(isolated_vertex_candidates)
  
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

      IF(MY_WITH_EXTERNALS)
        SET(ctk_dependencies ${dependencies})
      ELSE()
        # filter dependencies starting with CTK org org_commontk_
        ctkMacroGetAllCTKTargetLibraries("${dependencies}" ctk_dependencies)
      ENDIF()

      IF(ctk_dependencies)
        LIST(APPEND vertices ${target_project_name})
      ELSE()
        # isolated vertex candidate
        LIST(APPEND isolated_vertex_candidates ${target_project_name})
      ENDIF()

      # Generate XML related to the dependencies
      FOREACH(dependency_name ${ctk_dependencies})
        LIST(APPEND edges ${dependency_name})
        SET(dgraph_list ${dgraph_list} "${target_project_name} ${dependency_name}\n")
        LIST(APPEND vertices ${dependency_name})
      ENDFOREACH()

    ENDIF()
    
  ENDFOREACH()

  FOREACH(isolated_vertex_candidate ${isolated_vertex_candidates})
    SET(_found 0)
    FOREACH(dgraph_entry ${dgraph_list})
      STRING(REPLACE "\n" "" dgraph_entry "${dgraph_entry}")
      STRING(REPLACE " " ";" dgraph_entry "${dgraph_entry}")
      LIST(FIND dgraph_entry ${isolated_vertex_candidate} _index)
      IF(_index GREATER -1)
        SET(_found 1)
        BREAK()
      ENDIF()
    ENDFOREACH()

    IF(NOT _found)
      LIST(APPEND vertices ${isolated_vertex_candidate})
      SET(dgraph_list "${isolated_vertex_candidate}\n" ${dgraph_list})
    ENDIF()
  ENDFOREACH()

  IF(vertices)
    LIST(REMOVE_DUPLICATES vertices)
  ENDIF()
  LIST(LENGTH vertices numberOfVertices)
  LIST(LENGTH edges numberOfEdges)

  SET(dgraph_list "${numberOfVertices} ${numberOfEdges}\n" ${dgraph_list})

  IF(MY_WITH_OPTION)
    SET(filename "${dir}/DGraphInput.txt")
  ELSEIF(MY_WITH_EXTERNALS)
    SET(filename "${dir}/DGraphInput-alldep-withext.txt")
  ELSE()
    SET(filename "${dir}/DGraphInput-alldep.txt")
  ENDIF()

  FILE(WRITE ${filename} ${dgraph_list})
  MESSAGE(STATUS "Generated: ${filename}")
ENDFUNCTION()
