
#
#
#
MACRO(ctkMacroGenerateDGraphInput dir name target_directories)
  IF(NOT EXISTS ${dir})
    MESSAGE(FATAL_ERROR "Directory ${dir} doesn't exist!")
  ENDIF()

  SET(dgraph_list )

  SET(edges)
  SET(vertices)
  
  FOREACH(target_info ${target_directories})

    # extract target_dir and option_name
    STRING(REPLACE "::" "\\;" target_info ${target_info})
    SET(target_info_list ${target_info})
    LIST(GET target_info_list 0 target_dir)
    LIST(GET target_info_list 1 option_name)
    #MESSAGE(STATUS target_dir:${target_dir})
    #MESSAGE(STATUS option_name:${option_name})
    
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
    
    LIST(APPEND vertices ${target_project_name})

    # Make sure the variable is cleared
    SET(dependencies )

    # get dependencies
    ctkMacroCollectTargetLibraryNames(${target_dir} ${option_name} dependencies)
    
    # Make sure the variable is cleared
    SET(ctk_dependencies)

    # filter dependencies starting with CTK
    ctkMacroGetAllCTKTargetLibraries("${dependencies}" ctk_dependencies)
    
    # Generate XML related to the dependencies
    FOREACH(dependency_name ${ctk_dependencies})
      LIST(APPEND edges ${dependency_name})
      SET(dgraph_list ${dgraph_list} "${target_project_name} ${dependency_name}\n")
    ENDFOREACH()
    
  ENDFOREACH()

  LIST(LENGTH vertices numberOfVertices)
  LIST(LENGTH edges numberOfEdges)

  SET(dgraph_list "${numberOfVertices} ${numberOfEdges}\n" ${dgraph_list})
  
  SET(filename "${dir}/DGraphInput.txt")

  FILE(WRITE ${filename} ${dgraph_list})
  MESSAGE(STATUS "Generated: ${filename}")
ENDMACRO()