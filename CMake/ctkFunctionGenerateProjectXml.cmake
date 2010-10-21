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
#
#

FUNCTION(ctkFunctionGenerateProjectXml dir name target_directories is_superbuild)
  IF(NOT EXISTS ${dir})
    MESSAGE(FATAL_ERROR "Directory ${dir} doesn't exist!")
  ENDIF()

  SET(xml_subprojects )

  IF(${is_superbuild})
    SET(xml_subprojects ${xml_subprojects} "  <SubProject name=\"SuperBuild\">\n")
  ENDIF()
  
  FOREACH(target_info ${target_directories})

    # extract target_dir and option_name
    STRING(REPLACE "^^" "\\;" target_info ${target_info})
    SET(target_info_list ${target_info})
    LIST(GET target_info_list 0 target_dir)
    LIST(GET target_info_list 1 option_name)
    #MESSAGE(STATUS target_dir:${target_dir})
    #MESSAGE(STATUS option_name:${option_name})
    
    # make sure the directory exists
    IF(NOT EXISTS ${target_dir}/CMakeLists.txt)
      MESSAGE(FATAL_ERROR "Target directory ${target_dir}/CMakeLists.txt doesn't exists !")
    ENDIF()

    # Remarks: Project.xml should contains all sub-project. That way
    # all dashboards should submit a similar file.
    #IF(${${option_name}})

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
      
      SET(xml_subprojects ${xml_subprojects} "  <SubProject name=\"${target_project_name}\">\n")

      # Make sure the variable is cleared
      SET(dependencies )

      # get dependencies
      ctkMacroCollectTargetLibraryNames(${target_dir} dependencies)
      
      # Make sure the variable is cleared
      SET(ctk_dependencies)

      # filter dependencies starting with CTK
      ctkMacroGetAllCTKTargetLibraries("${dependencies}" ctk_dependencies)

      IF(${is_superbuild})
        SET(xml_subprojects ${xml_subprojects} "    <Dependency name=\"SuperBuild\"/>\n")
      ENDIF()
      
      # Generate XML related to the dependencies 
      FOREACH(dependency_name ${ctk_dependencies})
        SET(xml_subprojects ${xml_subprojects} "    <Dependency name=\"${dependency_name}\"/>\n")
      ENDFOREACH()
      
      SET(xml_subprojects ${xml_subprojects} "  </SubProject>\n")
    #ENDIF()
  ENDFOREACH()
   
  SET(xml_content "<Project name=\"${name}\">\n${xml_subprojects}</Project>")
  SET(filename "${dir}/Project.xml")

  FILE(WRITE ${filename} ${xml_content})
  MESSAGE(STATUS "Generated: ${filename}")
ENDFUNCTION()
