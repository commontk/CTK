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
MACRO(ctkMacroGetOptionName target_directories_with_target_name target_name option_name_var)
  
  FOREACH(target_info ${target_directories_with_target_name})
    # extract target_dir and option_name
    #MESSAGE("target_info:${target_info}")
    STRING(REPLACE "^^" "\\;" target_info_list ${target_info})
    SET(target_info_list ${target_info_list})
    LIST(GET target_info_list 0 _target_dir)
    LIST(GET target_info_list 1 _option_name)
    LIST(GET target_info_list 2 _target_name)
    IF(${_target_name} STREQUAL ${target_name})
      SET(${option_name_var} ${_option_name})
    ENDIF()
  ENDFOREACH()
ENDMACRO()

#
#
#
MACRO(ctkMacroValidateBuildOptions dir executable target_directories)
  IF(NOT EXISTS ${dir})
    MESSAGE(FATAL_ERROR "Directory ${dir} doesn't exist!")
  ENDIF()

  IF(NOT EXISTS ${executable})
    MESSAGE(FATAL_ERROR "Executable ${executable} doesn't exist!")
  ENDIF()

  SET(known_targets)
  SET(target_directories_with_target_name)
  
  # Create target_directories_with_target_name
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

    LIST(APPEND target_directories_with_target_name
      ${target_dir}^^${option_name}^^${target_project_name}
      )

    LIST(APPEND known_targets ${target_project_name})
  ENDFOREACH()

  # This is for external projects using CTK
  # The variables CTK_PLUGIN_LIBRARIES and CTK_LIBRARIES are set in CTKConfig.cmake
  IF(CTK_PLUGIN_LIBRARIES)
    LIST(APPEND known_targets ${CTK_PLUGIN_LIBRARIES})
  ENDIF()
  IF(CTK_LIBRARIES)
    LIST(APPEND known_targets ${CTK_LIBRARIES})
  ENDIF()

  #MESSAGE("Known targets: ${known_targets}")

  SET(EXTERNAL_TARGETS ) # This is used in CMakeLists.txt
  FOREACH(target_info ${target_directories_with_target_name})

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

    IF(${${option_name}})
      # Obtain dependency path
      ctkMacroSetPaths("${QT_INSTALLED_LIBRARY_DIR}")
      EXECUTE_PROCESS(
        COMMAND "${executable}" "${dir}/DGraphInput-alldep-withext.txt" -sort ${target_project_name}
        WORKING_DIRECTORY ${dir}
        RESULT_VARIABLE RESULT_VAR
        OUTPUT_VARIABLE dep_path
        ERROR_VARIABLE error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      IF(RESULT_VAR)
        MESSAGE(FATAL_ERROR "Failed to obtain dependence path of ${target_project_name}.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
      ENDIF()

      # Set a variable for each target containing its dependencies
      # Needed for setting individual include directories for plugins,
      # depending on other plugins.
      SET(${target_project_name}_DEPENDENCIES )

      # Convert 'dep_path' to a list
      STRING(REPLACE " " "\\;" dep_path_list ${dep_path})
      SET(dep_path_list ${dep_path_list})
      LIST(REMOVE_ITEM dep_path_list ${target_project_name})
      LIST(APPEND ${target_project_name}_DEPENDENCIES ${dep_path_list})

      #MESSAGE("path for ${target_project_name} is: ${dep_path_list}")

      # Check if all internal CTK targets included in the dependency path are enabled
      SET(int_dep_path_list )
      SET(ext_dep_path_list ${dep_path_list})
      # Allow external projects to override the set of internal targets
      IF(COMMAND GetMyTargetLibraries)
        GetMyTargetLibraries("${dep_path_list}" int_dep_path_list)
      ELSE()
        ctkMacroGetAllCTKTargetLibraries("${dep_path_list}" int_dep_path_list)
      ENDIF()
      IF(int_dep_path_list)
        LIST(REMOVE_ITEM ext_dep_path_list ${int_dep_path_list})
      ENDIF()

      IF(ext_dep_path_list)
        LIST(APPEND EXTERNAL_TARGETS ${ext_dep_path_list})
      ENDIF()

      FOREACH(dep ${int_dep_path_list})
        LIST(FIND known_targets ${dep} dep_found)
        IF(dep_found LESS 0)
          MESSAGE(FATAL_ERROR "${target_project_name} depends on ${dep}, which does not exist")
        ENDIF()

        ctkMacroGetOptionName("${target_directories_with_target_name}" ${dep} dep_option)
        IF(NOT ${${dep_option}})
          # Enable option
          MESSAGE(STATUS "Enabling option [${dep_option}] required by [${target_project_name}]")
          SET(${dep_option} ON CACHE BOOL "Enable ${target_project_name} library" FORCE)
        ENDIF()
      ENDFOREACH()
    ENDIF()
    
  ENDFOREACH()

  IF(EXTERNAL_TARGETS)
    LIST(REMOVE_DUPLICATES EXTERNAL_TARGETS)
  ENDIF()

  #MESSAGE(STATUS "Validated: CTK_LIB_*, CTK_PLUGIN_*, CTK_APP_*")
ENDMACRO()
