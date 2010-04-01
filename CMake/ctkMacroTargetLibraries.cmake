
#
#
#
MACRO(ctkMacroGetTargetLibraries varname)

  SET(filepath ${CMAKE_CURRENT_SOURCE_DIR}/target_libraries.cmake)

  # Check if "target_libraries.cmake" file exists
  IF(NOT EXISTS ${filepath})
    MESSAGE(FATAL_ERROR "${filepath} doesn't exists !")
  ENDIF()

  # Make sure the variable is cleared
  SET(target_libraries )

  # Let's make sure target_libraries contains only strings
  FILE(STRINGS "${filepath}" stringtocheck) # read content of 'filepath' into 'stringtocheck'
  STRING(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
  FOREACH(incorrect_element ${incorrect_elements})
    STRING(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
    MESSAGE(FATAL_ERROR "In ${filepath}, ${incorrect_element} should be replaced by ${correct_element}")
  ENDFOREACH()

  INCLUDE(${filepath})

  # Loop over all target library, if it does *NOT* start with "CTK",
  # let's resolve the variable to access its content
  FOREACH(target_library ${target_libraries})
    IF(${target_library} MATCHES "^CTK[a-zA-Z0-9]+$")
      LIST(APPEND ${varname} ${target_library})
    ELSE()
      LIST(APPEND ${varname} "${${target_library}}")
    ENDIF()
  ENDFOREACH()
ENDMACRO()

#
#
#
MACRO(ctkMacroCollectTargetLibraryNames target_dir option_name varname)

  #MESSAGE(STATUS option_name:${option_name})
  IF(${${option_name}})
    #MESSAGE(STATUS target:${target})
    SET(lib_targets)

    SET(filepath ${target_dir}/target_libraries.cmake)
    #MESSAGE(STATUS filepath:${filepath})

    # Check if "target_libraries.cmake" file exists
    IF(NOT EXISTS ${filepath})
      MESSAGE(FATAL_ERROR "${filepath} doesn't exists !")
    ENDIF()

    # Let's make sure target_libraries contains only strings
    FILE(STRINGS "${filepath}" stringtocheck)
    STRING(REGEX MATCHALL "[^#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    FOREACH(incorrect_element ${incorrect_elements})
      STRING(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      MESSAGE(FATAL_ERROR "In ${filepath}, ${incorrect_element} should be replaced by ${correct_element}")
    ENDFOREACH()

    # Make sure the variable is cleared
    SET(target_libraries)

    INCLUDE(${filepath})
    
    LIST(APPEND ${varname} ${target_libraries})
    LIST(REMOVE_DUPLICATES ${varname})
  ENDIF()
ENDMACRO()

#
#
#
MACRO(ctkMacroCollectAllTargetLibraries targets subdir varname)

  SET(option_prefix)
  IF(${subdir} STREQUAL "Libs")
    SET(option_prefix CTK_LIB_)
  ELSEIF(${subdir} STREQUAL "Plugins")
    SET(option_prefix CTK_PLUGIN_)
  ELSEIF(${subdir} STREQUAL "Applications")
    SET(option_prefix CTK_APP_)
  ELSE()
    MESSAGE(FATAL_ERROR "Unknown subdir:${subdir}, expected value are: 'Libs, 'Plugins' or 'Applications'")
  ENDIF()
  
  FOREACH(target ${targets})

    # Make sure the variable is cleared
    SET(target_libraries )

    SET(option_name ${option_prefix}${target})
    #MESSAGE(STATUS option_name:${option_name})

    SET(target_dir "${CTK_SOURCE_DIR}/${subdir}/${target}")
    #MESSAGE(STATUS target_dir:${target_dir})
    
    ctkMacroCollectTargetLibraryNames(${target_dir} ${option_name} target_libraries)

    LIST(APPEND ${varname} ${target_libraries})
    LIST(REMOVE_DUPLICATES ${varname})
  ENDFOREACH()
  
ENDMACRO()

#
# Extract all library names starting with CTK uppercase
#
MACRO(ctkMacroGetAllCTKTargetLibraries all_target_libraries varname)
  SET(re_ctklib "^CTK[a-zA-Z0-9]+$")
  SET(_tmp_list)
  LIST(APPEND _tmp_list ${all_target_libraries})
  ctkMacroListFilter(_tmp_list re_ctklib OUTPUT_VARIABLE ${varname})
  #MESSAGE(STATUS varname:${varname}:${${varname}})
ENDMACRO()

#
# Extract all library names *NOT* starting with CTK uppercase
#
MACRO(ctkMacroGetAllNonCTKTargetLibraries all_target_libraries varname)
  ctkMacroGetAllCTKTargetLibraries("${all_target_libraries}" all_ctk_libraries)
  SET(_tmp_list ${all_target_libraries})
  LIST(REMOVE_ITEM _tmp_list ${all_ctk_libraries})
  SET(${varname} ${_tmp_list})
  #MESSAGE(STATUS varname:${varname}:${${varname}})
ENDMACRO()

#
# 
#
MACRO(ctkMacroShouldAddExternalProject libraries_variable_name resultvar)
  IF(NOT DEFINED NON_CTK_DEPENDENCIES)
    MESSAGE(FATAL_ERROR "Variable NON_CTK_DEPENDENCIES is undefined !")
  ENDIF() 
  LIST(FIND NON_CTK_DEPENDENCIES ${libraries_variable_name} index)
  SET(${resultvar} FALSE)
  IF(${index} GREATER -1)
    SET(${resultvar} TRUE)
  ENDIF()
ENDMACRO()
