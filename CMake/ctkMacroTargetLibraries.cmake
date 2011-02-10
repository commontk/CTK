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
# This macro could be invoked using two different signatures:
#   ctkFunctionGetTargetLibraries(TARGET_LIBS)
# or 
#   ctkFunctionGetTargetLibraries(TARGET_LIBS "/path/to/ctk_target_dir")
#
# Without specifying the second argument, the current folder will be used.
#
FUNCTION(ctkFunctionGetTargetLibraries varname)

  SET(expanded_target_library_list)

  SET(TARGET_DIRECTORY ${ARGV1})
  IF("${TARGET_DIRECTORY}" STREQUAL "")
    SET(TARGET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  ENDIF()

  SET(filepath ${TARGET_DIRECTORY}/target_libraries.cmake)
  SET(manifestpath ${TARGET_DIRECTORY}/manifest_headers.cmake)

  # Check if "target_libraries.cmake" or "manifest_headers.cmake" file exists
  IF(NOT EXISTS ${filepath} AND NOT EXISTS ${manifestpath})
    MESSAGE(FATAL_ERROR "${filepath} or ${manifestpath} doesn't exists !")
  ENDIF()

  # Make sure the variable is cleared
  SET(target_libraries )
  SET(Require-Plugin )

  IF(EXISTS ${filepath})
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
      IF(${target_library} MATCHES "^CTK[a-zA-Z0-9]+$" OR
         ${target_library} MATCHES "^org_commontk_[a-zA-Z0-9_]+$")
        LIST(APPEND expanded_target_library_list ${target_library})
      ELSE()
        LIST(APPEND expanded_target_library_list "${${target_library}}")
      ENDIF()
    ENDFOREACH()
  ENDIF()

  IF(EXISTS ${manifestpath})
    # Let's make sure Require-Plugins contains only strings
    FILE(STRINGS "${manifestpath}" stringtocheck) # read content of 'manifestpath' into 'stringtocheck'
    STRING(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    FOREACH(incorrect_element ${incorrect_elements})
      STRING(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      MESSAGE(FATAL_ERROR "In ${manifestpath}, ${incorrect_element} should be replaced by ${correct_element}")
    ENDFOREACH()

    INCLUDE(${manifestpath})

    # Loop over all plugin dependencies,
    FOREACH(plugin_symbolicname ${Require-Plugin})
      STRING(REPLACE "." "_" plugin_library ${plugin_symbolicname})
      LIST(APPEND expanded_target_library_list ${plugin_library})
    ENDFOREACH()
  ENDIF()
  
  # Pass the list of target libraries to the caller
  SET(${varname} ${expanded_target_library_list} PARENT_SCOPE)

ENDFUNCTION()

#
#
#
FUNCTION(ctkFunctionCollectTargetLibraryNames target_dir varname)

  SET(target_library_list)
  #MESSAGE(STATUS target:${target})
  SET(lib_targets)

  SET(filepath ${target_dir}/target_libraries.cmake)
  SET(manifestpath ${target_dir}/manifest_headers.cmake)

  # Check if "target_libraries.cmake" or "manifest_headers.cmake" file exists
  IF(NOT EXISTS ${filepath} AND NOT EXISTS ${manifestpath})
    MESSAGE(FATAL_ERROR "${filepath} or ${manifestpath} doesn't exists !")
  ENDIF()

  # Make sure the variable is cleared
  SET(target_libraries )
  SET(Require-Plugin )

  IF(EXISTS ${filepath})
    # Let's make sure target_libraries contains only strings
    FILE(STRINGS "${filepath}" stringtocheck) # read content of 'filepath' into 'stringtocheck'
    STRING(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    FOREACH(incorrect_element ${incorrect_elements})
      STRING(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      MESSAGE(FATAL_ERROR "In ${filepath}, ${incorrect_element} should be replaced by ${correct_element}")
    ENDFOREACH()

    INCLUDE(${filepath})

    LIST(APPEND target_library_list ${target_libraries})
  ENDIF()

  IF(EXISTS ${manifestpath})
    # Let's make sure Require-Plugins contains only strings
    FILE(STRINGS "${manifestpath}" stringtocheck) # read content of 'manifestpath' into 'stringtocheck'
    STRING(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    FOREACH(incorrect_element ${incorrect_elements})
      STRING(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      MESSAGE(FATAL_ERROR "In ${manifestpath}, ${incorrect_element} should be replaced by ${correct_element}")
    ENDFOREACH()

    INCLUDE(${manifestpath})

    # Loop over all plugin dependencies
    FOREACH(plugin_symbolicname ${Require-Plugin})
      STRING(REPLACE "." "_" plugin_library ${plugin_symbolicname})
      LIST(APPEND target_library_list ${plugin_library})
    ENDFOREACH()
  ENDIF()

  LIST(REMOVE_DUPLICATES target_library_list)
  
  # Pass the list of target libraries to the caller
  SET(${varname} ${target_library_list} PARENT_SCOPE)
ENDFUNCTION()

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

    SET(target_libraries)
    
    # Collect target libraries only if option is ON
    IF(${option_name})
      ctkFunctionCollectTargetLibraryNames(${target_dir} target_libraries)
    ENDIF()

    IF(target_libraries)
      LIST(APPEND ${varname} ${target_libraries})
      LIST(REMOVE_DUPLICATES ${varname})
    ENDIF()
  ENDFOREACH()
  
ENDMACRO()

#
# Extract all library names starting with CTK uppercase or org_commontk_
#
MACRO(ctkMacroGetAllCTKTargetLibraries all_target_libraries varname)
  SET(re_ctklib "^(c|C)(t|T)(k|K)[a-zA-Z0-9]+$")
  SET(re_ctkplugin "^org_commontk_[a-zA-Z0-9_]+$")
  SET(_tmp_list)
  LIST(APPEND _tmp_list ${all_target_libraries})
  #MESSAGE("calling ctkMacroListFilter with varname:${varname}")
  ctkMacroListFilter(_tmp_list re_ctklib re_ctkplugin OUTPUT_VARIABLE ${varname})
  #MESSAGE(STATUS "getallctklibs from ${all_target_libraries}")
  #MESSAGE(STATUS varname:${varname}:${${varname}})
ENDMACRO()

#
# Extract all library names *NOT* starting with CTK uppercase or org_commontk_
#
MACRO(ctkMacroGetAllNonCTKTargetLibraries all_target_libraries varname)
  ctkMacroGetAllCTKTargetLibraries("${all_target_libraries}" all_ctk_libraries)
  SET(_tmp_list ${all_target_libraries})
  IF(all_ctk_libraries)
    LIST(REMOVE_ITEM _tmp_list ${all_ctk_libraries})
  ENDIF()
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
