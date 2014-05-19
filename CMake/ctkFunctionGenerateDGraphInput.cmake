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
#! Generate a DGrapgh input file expected by DGraph executable.
#!
#! \ingroup CMakeAPI
function(ctkFunctionGenerateDGraphInput dir target_directories)
  if(NOT EXISTS ${dir})
    message(FATAL_ERROR "Directory ${dir} doesn't exist!")
  endif()

  CtkMacroParseArguments(MY
    ""
    "WITH_OPTION;WITH_EXTERNALS"
    ${ARGN}
    )

  set(dgraph_list )

  set(edges)
  set(vertices)
  set(isolated_vertex_candidates)
  
  foreach(target_info ${target_directories})

    # extract target_dir and option_name
    string(REPLACE "^^" "\\;" target_info ${target_info})
    set(target_info_list ${target_info})
    list(GET target_info_list 0 target_dir)
    list(GET target_info_list 1 option_name)
    #message(STATUS target_dir:${target_dir})
    #message(STATUS option_name:${option_name})
    #message(STATUS option_name-value:${${option_name}})
    
    # make sure the directory exists
    if(NOT EXISTS ${target_dir}/CMakeLists.txt)
      message(FATAL_ERROR "Target directory ${target_dir}/CMakeLists.txt doesn't exists !")
    endif()

    set(include_dep 1)
    if(MY_WITH_OPTION)
      set(include_dep ${${option_name}})
    endif()
    if(${include_dep})
      # extract project name from CMakeLists.txt
      file(STRINGS "${target_dir}/CMakeLists.txt" project_string
        REGEX "^ *(P|p)(R|r)(O|o)(J|j)(E|e)(C|c)(T|t)\\("
        LIMIT_COUNT 10)
      string(REGEX MATCH "\\((.*)\\)" target_project_name ${project_string})
      string(REGEX REPLACE "\\(|\\)" "" target_project_name ${target_project_name})
      if(${target_project_name} STREQUAL "")
        message(FATAL_ERROR "Failed to extract project name from ${target_dir}/CMakeLists.txt")
      endif()
      #message(STATUS target_project_name:${target_project_name})

      # Make sure the variable is cleared
      set(dependencies )

      # get dependencies
      ctkFunctionCollectTargetLibraryNames(${target_dir} dependencies)
      if(${target_project_name}_OPTIONAL_DEPENDENCIES)
        list(APPEND dependencies ${${target_project_name}_OPTIONAL_DEPENDENCIES})
      endif()

      # Make sure the variable is cleared
      set(ctk_dependencies)

      if(MY_WITH_EXTERNALS)
        set(ctk_dependencies ${dependencies})
      else()
        # filter dependencies starting with CTK org org_commontk_
        ctkMacroGetAllProjectTargetLibraries("${dependencies}" ctk_dependencies)
      endif()

      if(ctk_dependencies)
        list(APPEND vertices ${target_project_name})
      else()
        # isolated vertex candidate
        list(APPEND isolated_vertex_candidates ${target_project_name})
      endif()

      # Generate XML related to the dependencies
      foreach(dependency_name ${ctk_dependencies})
        list(APPEND edges ${dependency_name})
        set(dgraph_list ${dgraph_list} "${target_project_name} ${dependency_name}\n")
        list(APPEND vertices ${dependency_name})
      endforeach()

    endif()
    
  endforeach()

  foreach(isolated_vertex_candidate ${isolated_vertex_candidates})
    set(_found 0)
    foreach(dgraph_entry ${dgraph_list})
      string(REPLACE "\n" "" dgraph_entry "${dgraph_entry}")
      string(REPLACE " " ";" dgraph_entry "${dgraph_entry}")
      list(FIND dgraph_entry ${isolated_vertex_candidate} _index)
      if(_index GREATER -1)
        set(_found 1)
        break()
      endif()
    endforeach()

    if(NOT _found)
      list(APPEND vertices ${isolated_vertex_candidate})
      set(dgraph_list "${isolated_vertex_candidate}\n" ${dgraph_list})
    endif()
  endforeach()

  if(vertices)
    list(REMOVE_DUPLICATES vertices)
  endif()
  list(LENGTH vertices numberOfVertices)
  list(LENGTH edges numberOfEdges)

  set(dgraph_list "${numberOfVertices} ${numberOfEdges}\n" ${dgraph_list})

  if(MY_WITH_OPTION)
    set(filename "${dir}/DGraphInput.txt")
  elseif(MY_WITH_EXTERNALS)
    set(filename "${dir}/DGraphInput-alldep-withext.txt")
  else()
    set(filename "${dir}/DGraphInput-alldep.txt")
  endif()

  file(WRITE ${filename} ${dgraph_list})
  message(STATUS "Generated: ${filename}")
endfunction()
