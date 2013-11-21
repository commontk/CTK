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

macro(ctk_include_once)
  # Make sure this file is included only once
  get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
  if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
    return()
  endif()
  set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)
endmacro()

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

  # Keep track of the projects
  list(APPEND __epd_${CMAKE_PROJECT_NAME}_projects ${proj})

  # Is this the first run ? (used to set the <CMAKE_PROJECT_NAME>_USE_SYSTEM_* variables)
  if(${proj} STREQUAL ${CMAKE_PROJECT_NAME} AND NOT DEFINED __epd_first_pass)
    message(STATUS "SuperBuild - First pass")
    set(__epd_first_pass TRUE)
  endif()

  # Set message strings
  set(__${proj}_indent ${__indent})
  set(__${proj}_superbuild_message "SuperBuild - ${__indent}${proj}[OK]")
  if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
    set(__${proj}_superbuild_message "${__${proj}_superbuild_message} (SYSTEM)")
  endif()

  # Display dependency of project being processed
  if("${${proj}_DEPENDENCIES}" STREQUAL "")
    _epd_status(${__${proj}_superbuild_message})
  else()
    set(dependency_str " ")
    foreach(dep ${${proj}_DEPENDENCIES})
      if(${EXTERNAL_PROJECT_FILE_PREFIX}${dep}_FILE_INCLUDED)
        set(dependency_str "${dependency_str}${dep}[INCLUDED], ")
      else()
        set(dependency_str "${dependency_str}${dep}, ")
      endif()
    endforeach()
    _epd_status("SuperBuild - ${__indent}${proj} => Requires${dependency_str}")
  endif()

  foreach(dep ${${proj}_DEPENDENCIES})
    if(${${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}})
      set(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${dep} ${${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}})
    endif()
    #if(__epd_first_pass)
    #  message("${CMAKE_PROJECT_NAME}_USE_SYSTEM_${dep} set to [${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}:${${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}}]")
    #endif()
  endforeach()

  # Include dependencies
  foreach(dep ${${proj}_DEPENDENCIES})
    if(NOT External_${dep}_FILE_INCLUDED)
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

  if(${proj} STREQUAL ${CMAKE_PROJECT_NAME} AND __epd_first_pass)
    message(STATUS "SuperBuild - First pass - done")
    unset(__indent)
    if(${CMAKE_PROJECT_NAME}_SUPERBUILD)
      set(__epd_first_pass FALSE)
    endif()

    unset(${CMAKE_PROJECT_NAME}_DEPENDENCIES) # XXX - Refactor

    foreach(possible_proj ${__epd_${CMAKE_PROJECT_NAME}_projects})
      if(NOT ${possible_proj} STREQUAL ${CMAKE_PROJECT_NAME})

        unset(${EXTERNAL_PROJECT_FILE_PREFIX}${possible_proj}_FILE_INCLUDED)

        # XXX - Refactor - The following code should be re-organized
        if(DEFINED ${possible_proj}_enabling_variable)
          ctkMacroShouldAddExternalproject(${${possible_proj}_enabling_variable} add_project)
          if(${add_project})
            list(APPEND ${CMAKE_PROJECT_NAME}_DEPENDENCIES ${possible_proj})
          else()
            # XXX HACK
            if(${possible_proj} STREQUAL "VTK"
               AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
              list(APPEND ${CMAKE_PROJECT_NAME}_DEPENDENCIES VTK)
            else()
              unset(${${possible_proj}_enabling_variable}_INCLUDE_DIRS)
              unset(${${possible_proj}_enabling_variable}_LIBRARY_DIRS)
              unset(${${possible_proj}_enabling_variable}_FIND_PACKAGE_CMD)
              if(${CMAKE_PROJECT_NAME}_SUPERBUILD)
                message(STATUS "SuperBuild - ${possible_proj}[OPTIONAL]")
              endif()
            endif()
          endif()
        else()
          list(APPEND ${CMAKE_PROJECT_NAME}_DEPENDENCIES ${possible_proj})
        endif()
        # XXX

      else()

      endif()
    endforeach()

    list(REMOVE_DUPLICATES ${CMAKE_PROJECT_NAME}_DEPENDENCIES)

    if(${CMAKE_PROJECT_NAME}_SUPERBUILD)
      ctkMacroCheckExternalProjectDependency(${CMAKE_PROJECT_NAME})
    endif()

  endif()

  if(__epd_first_pass)
    return()
  endif()
endmacro()
