
#
#
#

MACRO(ctkMacroGetTargetLibraries varname)

  SET(target_libraries_path ${CMAKE_CURRENT_SOURCE_DIR}/target_libraries.cmake)
  
  # Check if "target_libraries.cmake" file exists
  IF(NOT EXISTS ${target_libraries_path})
    MESSAGE(FATAL_ERROR "${target_libraries_path} doesn't exists !")
  ENDIF()

  # Make sure the variable is cleared 
  SET(target_libraries )

  INCLUDE(${target_libraries_path})

  SET(${varname} ${target_libraries})
ENDMACRO()