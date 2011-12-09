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

#! \ingroup CMakeUtilities
macro(ctkMacroGetOptionName target_directories_with_target_name target_name option_name_var)
  
  foreach(target_info ${target_directories_with_target_name})
    # extract target_dir and option_name
    #message("target_info:${target_info}")
    string(REPLACE "^^" "\\;" target_info_list ${target_info})
    set(target_info_list ${target_info_list})
    list(GET target_info_list 0 _target_dir)
    list(GET target_info_list 1 _option_name)
    list(GET target_info_list 2 _target_name)
    if(${_target_name} STREQUAL ${target_name})
      set(${option_name_var} ${_option_name})
    endif()
  endforeach()
endmacro()

#! \ingroup CMakeUtilities
macro(ctkMacroValidateBuildOptions dir executable target_directories)
  if(NOT EXISTS ${dir})
    message(FATAL_ERROR "Directory ${dir} doesn't exist!")
  endif()

  if(NOT EXISTS ${executable})
    message(FATAL_ERROR "Executable ${executable} doesn't exist!")
  endif()

  set(known_targets)
  set(target_directories_with_target_name)
  
  # Create target_directories_with_target_name
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

    list(APPEND target_directories_with_target_name
      ${target_dir}^^${option_name}^^${target_project_name}
      )

    list(APPEND known_targets ${target_project_name})
  endforeach()

  # This is for external projects using CTK
  # The variable CTK_EXTERNAL_PLUGIN_LIBRARIES is filled in ctkMacroSetupExternalPlugins
  # with the help of variables defined in "PluginUseFiles" from external projects.
  if(CTK_EXTERNAL_PLUGIN_LIBRARIES)
    list(APPEND known_targets ${CTK_EXTERNAL_PLUGIN_LIBRARIES})
  endif()
  # The variable CTK_LIBRARIES is set in CTKConfig.cmake
  if(CTK_LIBRARIES)
    list(APPEND known_targets ${CTK_LIBRARIES})
  endif()

  #message("Known targets: ${known_targets}")

  set(EXTERNAL_TARGETS ) # This is used in CMakeLists.txt
  foreach(target_info ${target_directories_with_target_name})

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

    # Obtain dependency path
    ctkMacroSetPaths("${QT_INSTALLED_LIBRARY_DIR}")
    execute_process(
      COMMAND "${executable}" "${dir}/DGraphInput-alldep-withext.txt" -sort ${target_project_name}
      WORKING_DIRECTORY ${dir}
      RESULT_VARIABLE RESULT_VAR
      OUTPUT_VARIABLE dep_path
      ERROR_VARIABLE error
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(RESULT_VAR)
      message(FATAL_ERROR "Failed to obtain dependence path of ${target_project_name}.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
    endif()

    # Set a variable for each target containing its dependencies
    # Needed for setting individual include directories for plugins,
    # depending on other plugins.
    set(${target_project_name}_DEPENDENCIES )

    # Convert 'dep_path' to a list
    string(REPLACE " " "\\;" dep_path_list ${dep_path})
    set(dep_path_list ${dep_path_list})
    list(REMOVE_ITEM dep_path_list ${target_project_name})
    list(APPEND ${target_project_name}_DEPENDENCIES ${dep_path_list})

    #message("path for ${target_project_name} is: ${dep_path_list}")

    # Check if all internal CTK targets included in the dependency path are enabled
    set(int_dep_path_list )
    set(ext_dep_path_list ${dep_path_list})
    ctkMacroGetAllProjectTargetLibraries("${dep_path_list}" int_dep_path_list)
    if(int_dep_path_list)
      list(REMOVE_ITEM ext_dep_path_list ${int_dep_path_list})
    endif()

    if(ext_dep_path_list)
      list(APPEND EXTERNAL_TARGETS ${ext_dep_path_list})
      # If this macro is called from inside CTK itself, we add the external
      # targets to the list of known targets (for external projects calling
      # this macro, targets external to the calling project should be listed
      # in CTK_LIBRARIES or CTK_EXTERNAL_PLUGIN_LIBRARIES
      if(CTK_SOURCE_DIR)
        if(${CMAKE_SOURCE_DIR} STREQUAL ${CTK_SOURCE_DIR})
          list(APPEND known_targets ${ext_dep_path_list})
        endif()
      endif()
    endif()

    foreach(dep ${int_dep_path_list})
      list(FIND known_targets ${dep} dep_found)
      if(dep_found LESS 0)
        message(FATAL_ERROR "${target_project_name} depends on ${dep}, which does not exist")
      endif()

      ctkMacroGetOptionName("${target_directories_with_target_name}" ${dep} dep_option)
      if(${${option_name}} AND NOT ${${dep_option}})
        # Enable option
        message(STATUS "Enabling option [${dep_option}] required by [${target_project_name}]")
        set(${dep_option} ON CACHE BOOL "Enable ${target_project_name} library" FORCE)
      endif()
    endforeach()

    # For enabled targets, check if all external targets included in the dependency path are available
    if(${${option_name}})
      foreach(dep ${ext_dep_path_list})
        list(FIND known_targets ${dep} dep_found)
        if(dep_found LESS 0)
          message(FATAL_ERROR "${target_project_name} depends on unknown external targets: ${dep}")
        endif()
      endforeach()
    endif()
    
  endforeach()

  if(EXTERNAL_TARGETS)
    list(REMOVE_DUPLICATES EXTERNAL_TARGETS)
  endif()

  #message(STATUS "Validated: CTK_LIB_*, CTK_PLUGIN_*, CTK_APP_*")
endmacro()
