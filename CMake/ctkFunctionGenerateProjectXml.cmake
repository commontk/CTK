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
#
#

#! \ingroup CMakeUtilities
function(ctkFunctionGenerateProjectXml dir name target_directories is_superbuild)
  if(NOT EXISTS ${dir})
    message(FATAL_ERROR "Directory ${dir} doesn't exist!")
  endif()

  set(xml_subprojects )

  if(${is_superbuild})
    set(xml_subprojects ${xml_subprojects} "  <SubProject name=\"SuperBuild\">\n")
  endif()
  
  set(subproject_list)
  
  foreach(target_info ${target_directories})

    # extract target_dir and option_name
    string(REPLACE "^^" "\\;" target_info ${target_info})
    set(target_info_list ${target_info})
    list(GET target_info_list 0 target_dir)
    list(GET target_info_list 1 option_name)
    #message(STATUS target_dir:${target_dir})
    #message(STATUS option_name:${option_name})
    
    # make sure the directory exists
    if(NOT EXISTS ${target_dir}/CMakeLists.txt)
      message(FATAL_ERROR "Target directory ${target_dir}/CMakeLists.txt doesn't exists !")
    endif()

    # Remarks: Project.xml should contains all sub-project. That way
    # all dashboards should submit a similar file.
    #if(${${option_name}})

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
      
      set(xml_subprojects ${xml_subprojects} "  <SubProject name=\"${target_project_name}\">\n")
      list(APPEND subproject_list ${target_project_name})
      
      # Make sure the variable is cleared
      set(dependencies )

      # get dependencies
      ctkFunctionCollectTargetLibraryNames(${target_dir} dependencies)
      
      # Make sure the variable is cleared
      set(ctk_dependencies)

      # filter dependencies starting with CTK
      ctkMacroGetAllProjectTargetLibraries("${dependencies}" ctk_dependencies)

      if(${is_superbuild})
        set(xml_subprojects ${xml_subprojects} "    <Dependency name=\"SuperBuild\"/>\n")
      endif()
      
      # Generate XML related to the dependencies 
      foreach(dependency_name ${ctk_dependencies})
        set(xml_subprojects ${xml_subprojects} "    <Dependency name=\"${dependency_name}\"/>\n")
      endforeach()
      
      set(xml_subprojects ${xml_subprojects} "  </SubProject>\n")
    #endif()
  endforeach()
  
  set(xml_subprojects ${xml_subprojects} "  <SubProject name=\"Documentation\">\n")
  foreach(subproject ${subproject_list})
    set(xml_subprojects ${xml_subprojects} "    <Dependency name=\"${subproject}\"/>\n")
  endforeach()
  set(xml_subprojects ${xml_subprojects} "  </SubProject>\n")
   
  set(xml_content "<Project name=\"${name}\">\n${xml_subprojects}</Project>")
  set(filename "${dir}/Project.xml")

  file(WRITE ${filename} ${xml_content})
  message(STATUS "Generated: ${filename}")
endfunction()
