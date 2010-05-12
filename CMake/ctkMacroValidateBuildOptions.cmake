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
  ENDFOREACH()

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
      ctkFunctionExecuteProcess(
        COMMAND "${executable}" "${CTK_BINARY_DIR}/DGraphInput-alldep.txt" -paths ${target_project_name}
        PATH_LIST \"${QT_INSTALLED_LIBRARY_DIR}\"
        WORKING_DIRECTORY ${CTK_BINARY_DIR}
        RESULT_VARIABLE RESULT_VAR
        OUTPUT_VARIABLE dep_paths
        ERROR_VARIABLE error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      IF(RESULT_VAR)
        MESSAGE(FATAL_ERROR "Failed to obtain dependence path of ${subir}.\n${RESULT_VAR}\n${CTK_BINARY_DIR}\n${error}")
      ENDIF()

      FOREACH(dep_path ${dep_paths})

        # Convert 'dep_path' to a list
        STRING(REPLACE " " "\\;" dep_path_list ${dep_path})
        SET(dep_path_list ${dep_path_list})

        #MESSAGE("path for ${target_project_name} is: ${dep_path}")
        
        # Check if all target included in the dependency path are enabled
        FOREACH(dep ${dep_path_list})
          ctkMacroGetOptionName("${target_directories_with_target_name}" ${dep} dep_option)
          IF(NOT ${${dep_option}})
            # Enable option
            MESSAGE(STATUS "Enabling option [${dep_option}] required by [${target_project_name}]")
            SET(${dep_option} ON CACHE BOOL "Enable ${target_project_name} library" FORCE)
          ENDIF()
        ENDFOREACH()
      ENDFOREACH()
    ENDIF()
    
  ENDFOREACH()

  #MESSAGE(STATUS "Validated: CTK_LIB_*, CTK_PLUGIN_*, CTK_APP_*")
ENDMACRO()