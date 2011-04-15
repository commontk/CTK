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

# Write a set of variables containing CTK plugin specific include and library directories
FUNCTION(ctkFunctionGeneratePluginUseFile filename)
  IF(COMMAND GetMyTargetLibraries)
    GetMyTargetLibraries("${CTK_PLUGIN_LIBRARIES}" my_ctk_plugin_libraries)
  ELSE()
    SET(my_ctk_plugin_libraries ${CTK_PLUGIN_LIBRARIES})
  ENDIF()

  SET(CTK_PLUGIN_INCLUDE_DIRS_CONFIG)
  FOREACH(plugin ${my_ctk_plugin_libraries})
    SET(${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR} ${${plugin}_BINARY_DIR})
    ctkFunctionGetIncludeDirs(${plugin}_INCLUDE_DIRS ${plugin})
    SET(CTK_PLUGIN_INCLUDE_DIRS_CONFIG "${CTK_PLUGIN_INCLUDE_DIRS_CONFIG}
SET(${plugin}_INCLUDE_DIRS \"${${plugin}_INCLUDE_DIRS}\")")

    ctkFunctionGetLibraryDirs(${plugin}_LIBRARY_DIRS ${plugin})
    SET(CTK_PLUGIN_LIBRARY_DIRS_CONFIG "${CTK_PLUGIN_LIBRARY_DIRS_CONFIG}
SET(${plugin}_LIBRARY_DIRS \"${${plugin}_LIBRARY_DIRS}\")")
  ENDFOREACH()

  CONFIGURE_FILE("${CTK_CMAKE_DIR}/../CTKPluginUseFile.cmake.in" "${filename}" @ONLY)
ENDFUNCTION()
