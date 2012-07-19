###########################################################################
#
#  Library: CTK
#
#  Copyright (c) German Cancer Research Center,
#    Division of Medical and Biological Informatics
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#! Write a set of variables containing CTK plugin specific include and library directories
#! \ingroup CMakeAPI
function(ctkFunctionGeneratePluginUseFile filename)
  set(CTK_PLUGIN_INCLUDE_DIRS_CONFIG)

  foreach(plugin ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES})
    set(${plugin}_INCLUDE_DIRS )

    # The call to ctkFunctionGetIncludeDirs returns all include dirs
    # the plugin itself needs. This does not include the plugin's
    # source dir, so we add it explicitly.
    if(${plugin}_INCLUDE_SUFFIXES)
      foreach(_suffix ${${plugin}_INCLUDE_SUFFIXES})
        list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR}/${_suffix})
      endforeach()
    else()
      list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR})
    endif()

    list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_BINARY_DIR})

    ctkFunctionGetIncludeDirs(${plugin}_INCLUDE_DIRS ${plugin})

    set(CTK_PLUGIN_INCLUDE_DIRS_CONFIG "${CTK_PLUGIN_INCLUDE_DIRS_CONFIG}
set(${plugin}_INCLUDE_DIRS \"${${plugin}_INCLUDE_DIRS}\")")

    ctkFunctionGetLibraryDirs(${plugin}_LIBRARY_DIRS ${plugin})
    set(CTK_PLUGIN_LIBRARY_DIRS_CONFIG "${CTK_PLUGIN_LIBRARY_DIRS_CONFIG}
set(${plugin}_LIBRARY_DIRS \"${${plugin}_LIBRARY_DIRS}\")")
  endforeach()
  
  set(_ctk_plugin_libraries_variable ${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES)
  set(_ctk_plugin_libraries ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES})

  configure_file("${CTK_CMAKE_DIR}/../CMake/CTKPluginUseFile.cmake.in" "${filename}" @ONLY)
endfunction()
