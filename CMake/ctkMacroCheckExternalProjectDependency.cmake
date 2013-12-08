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

if(NOT EXISTS "${EXTERNAL_PROJECT_DIR}")
  set(EXTERNAL_PROJECT_DIR ${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/SuperBuild)
endif()

if(NOT DEFINED EXTERNAL_PROJECT_FILE_PREFIX)
  set(EXTERNAL_PROJECT_FILE_PREFIX "External_")
endif()

#
# superbuild_include_once()
#
# superbuild_include_once() is a macro intented to be used as include guard.
#
# It ensures that the CMake code placed after the include guard in a CMake file included
# using either 'include(/path/to/file.cmake)' or 'include(cmake_module)' will be executed
# once.
#
# It internally set the global property '<CMAKE_CURRENT_LIST_FILENAME>_FILE_INCLUDED' to check if
# a file has already been included.
#
macro(superbuild_include_once)
  # Make sure this file is included only once
  get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
  set(_property_name ${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  get_property(${_property_name} GLOBAL PROPERTY ${_property_name})
  if(${_property_name})
    return()
  endif()
  set_property(GLOBAL PROPERTY ${_property_name} 1)
endmacro()

#!
#! superbuild_cmakevar_to_cmakearg(<cmake_varname_and_type> <cmake_arg_var> <cmake_arg_type> [<varname_var> [<vartype_var>]])
#!
#! <cmake_varname_and_type> corresponds to variable name and variable type passed as "<varname>:<vartype>"
#!
#! <cmake_arg_var> is a variable name that will be set to "-D<varname>:<vartype>=${<varname>}"
#!
#! <cmake_arg_type> is set to either CMAKE_CACHE or CMAKE_CMD.
#!                  CMAKE_CACHE means that the generated cmake argument will be passed to
#!                  ExternalProject_Add as CMAKE_CACHE_ARGS.
#!                  CMAKE_CMD means that the generated cmake argument will be passed to
#!                  ExternalProject_Add as CMAKE_ARGS.
#!
#! <varname_var> is an optional variable name that will be set to "<varname>"
#!
#! <vartype_var> is an optional variable name that will be set to "<vartype>"
function(superbuild_cmakevar_to_cmakearg cmake_varname_and_type cmake_arg_var cmake_arg_type)
  set(_varname_var ${ARGV3})
  set(_vartype_var ${ARGV4})
  string(REPLACE ":" ";" varname_and_vartype ${cmake_varname_and_type})
  list(GET varname_and_vartype 0 _varname)
  list(GET varname_and_vartype 1 _vartype)
  set(_var_value "${${_varname}}")
  get_property(_value_set_in_cache CACHE ${_varname} PROPERTY VALUE SET)
  if(_value_set_in_cache)
    get_property(_var_value CACHE ${_varname} PROPERTY VALUE)
  endif()

  # XXX Add check for <cmake_arg_type> value

  if(cmake_arg_type STREQUAL "CMAKE_CMD")
    # Separate list item with <sep>
    set(ep_arg_as_string "")
    ctk_list_to_string(${sep} "${_var_value}" _var_value)
  endif()

  set(${cmake_arg_var} -D${_varname}:${_vartype}=${_var_value} PARENT_SCOPE)

  if(_varname_var MATCHES ".+")
    set(${_varname_var} ${_varname} PARENT_SCOPE)
  endif()
  if(_vartype_var MATCHES ".+")
    set(${_vartype_var} ${_vartype} PARENT_SCOPE)
  endif()
endfunction()

macro(_epd_status txt)
  if(NOT __epd_first_pass)
    message(STATUS ${txt})
  endif()
endmacro()

macro(ctkMacroCheckExternalProjectDependency proj)

  # Set indent variable if needed
  if(NOT DEFINED __indent)
    set(__indent "")
  else()
    set(__indent "${__indent}  ")
  endif()

  # Sanity checks
  if(NOT DEFINED ${proj}_DEPENDENCIES)
    message(FATAL_ERROR "${__indent}${proj}_DEPENDENCIES variable is NOT defined !")
  endif()

  if(NOT DEFINED SUPERBUILD_TOPLEVEL_PROJECT)
    set(SUPERBUILD_TOPLEVEL_PROJECT ${proj})
  endif()

  # Keep track of the projects
  list(APPEND __epd_${SUPERBUILD_TOPLEVEL_PROJECT}_projects ${proj})

  # Is this the first run ? (used to set the <SUPERBUILD_TOPLEVEL_PROJECT>_USE_SYSTEM_* variables)
  if(${proj} STREQUAL ${SUPERBUILD_TOPLEVEL_PROJECT} AND NOT DEFINED __epd_first_pass)
    message(STATUS "SuperBuild - First pass")
    set(__epd_first_pass TRUE)
  endif()

  # Set message strings
  set(__${proj}_indent ${__indent})
  set(__${proj}_superbuild_message "SuperBuild - ${__indent}${proj}[OK]")
  if(${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj})
    set(__${proj}_superbuild_message "${__${proj}_superbuild_message} (SYSTEM)")
  endif()

  # Display dependency of project being processed
  if("${${proj}_DEPENDENCIES}" STREQUAL "")
    _epd_status(${__${proj}_superbuild_message})
  else()
    set(dependency_str " ")
    foreach(dep ${${proj}_DEPENDENCIES})
      get_property(_is_included GLOBAL PROPERTY ${EXTERNAL_PROJECT_FILE_PREFIX}${dep}_FILE_INCLUDED)
      if(_is_included)
        set(dependency_str "${dependency_str}${dep}[INCLUDED], ")
      else()
        set(dependency_str "${dependency_str}${dep}, ")
      endif()
    endforeach()
    _epd_status("SuperBuild - ${__indent}${proj} => Requires${dependency_str}")
  endif()

  foreach(dep ${${proj}_DEPENDENCIES})
    if(${${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj}})
      set(${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${dep} ${${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj}})
    endif()
    #if(__epd_first_pass)
    #  message("${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${dep} set to [${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj}:${${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj}}]")
    #endif()
  endforeach()

  # Include dependencies
  foreach(dep ${${proj}_DEPENDENCIES})
    get_property(_is_included GLOBAL PROPERTY External_${dep}_FILE_INCLUDED)
    if(NOT _is_included)
      # XXX - Refactor - Add a single variable named 'EXTERNAL_PROJECT_DIRS'
      if(EXISTS "${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
        include(${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake)
      elseif(EXISTS "${${dep}_FILEPATH}")
        include(${${dep}_FILEPATH})
      elseif(EXISTS "${EXTERNAL_PROJECT_ADDITIONAL_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
        include(${EXTERNAL_PROJECT_ADDITIONAL_DIR}/${EXTERNAL_PEXCLUDEDROJECT_FILE_PREFIX}${dep}.cmake)
      else()
        message(FATAL_ERROR "Can't find ${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
      endif()
    endif()
  endforeach()

  # If project being process has dependencies, indicates it has also been added.
  if(NOT "${${proj}_DEPENDENCIES}" STREQUAL "")
    _epd_status(${__${proj}_superbuild_message})
  endif()

  # Update indent variable
  string(LENGTH "${__indent}" __indent_length)
  math(EXPR __indent_length "${__indent_length}-2")
  if(NOT ${__indent_length} LESS 0)
    string(SUBSTRING "${__indent}" 0 ${__indent_length} __indent)
  endif()

  if(${proj} STREQUAL ${SUPERBUILD_TOPLEVEL_PROJECT} AND __epd_first_pass)
    message(STATUS "SuperBuild - First pass - done")
    unset(__indent)
    if(${SUPERBUILD_TOPLEVEL_PROJECT}_SUPERBUILD)
      set(__epd_first_pass FALSE)
    endif()

    unset(${SUPERBUILD_TOPLEVEL_PROJECT}_DEPENDENCIES) # XXX - Refactor

    foreach(possible_proj ${__epd_${SUPERBUILD_TOPLEVEL_PROJECT}_projects})
      if(NOT ${possible_proj} STREQUAL ${SUPERBUILD_TOPLEVEL_PROJECT})

        set_property(GLOBAL PROPERTY ${EXTERNAL_PROJECT_FILE_PREFIX}${possible_proj}_FILE_INCLUDED 0)

        # XXX - Refactor - The following code should be re-organized
        if(DEFINED ${possible_proj}_enabling_variable)
          ctkMacroShouldAddExternalproject(${${possible_proj}_enabling_variable} add_project)
          if(${add_project})
            list(APPEND ${SUPERBUILD_TOPLEVEL_PROJECT}_DEPENDENCIES ${possible_proj})
          else()
            # XXX HACK
            if(${possible_proj} STREQUAL "VTK"
               AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
              list(APPEND ${SUPERBUILD_TOPLEVEL_PROJECT}_DEPENDENCIES VTK)
            else()
              unset(${${possible_proj}_enabling_variable}_INCLUDE_DIRS)
              unset(${${possible_proj}_enabling_variable}_LIBRARY_DIRS)
              unset(${${possible_proj}_enabling_variable}_FIND_PACKAGE_CMD)
              if(${SUPERBUILD_TOPLEVEL_PROJECT}_SUPERBUILD)
                message(STATUS "SuperBuild - ${possible_proj}[OPTIONAL]")
              endif()
            endif()
          endif()
        else()
          list(APPEND ${SUPERBUILD_TOPLEVEL_PROJECT}_DEPENDENCIES ${possible_proj})
        endif()
        # XXX

      else()

      endif()
    endforeach()

    list(REMOVE_DUPLICATES ${SUPERBUILD_TOPLEVEL_PROJECT}_DEPENDENCIES)

    if(${SUPERBUILD_TOPLEVEL_PROJECT}_SUPERBUILD)

      ctkMacroCheckExternalProjectDependency(${SUPERBUILD_TOPLEVEL_PROJECT})
    endif()

  endif()

  if(__epd_first_pass)
    return()
  endif()
endmacro()
