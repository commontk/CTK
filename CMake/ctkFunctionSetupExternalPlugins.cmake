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

FUNCTION(ctkFunctionSetupExternalPlugins )

  ctkMacroParseArguments(MY "BUILD_OPTION_PREFIX" "" ${ARGN})

  IF(NOT MY_DEFAULT_ARGS)
    MESSAGE(FATAL_ERROR "Empty plugin list")
  ENDIF()

  SET(plugin_list ${MY_DEFAULT_ARGS})

  IF(NOT MY_BUILD_OPTION_PREFIX)
    SET(MY_BUILD_OPTION_PREFIX "BUILD_")
  ENDIF()

  SET(plugin_dirswithoption )
  SET(plugin_subdirs )
  FOREACH(plugin ${plugin_list})
    ctkFunctionExtractOptionNameAndValue(${plugin} plugin_name plugin_value)
    SET(option_name ${MY_BUILD_OPTION_PREFIX}${plugin_name})
    OPTION(${option_name} "Build ${plugin_name} Plugin." ${plugin_value})
    LIST(APPEND plugin_subdirs "${plugin_name}")
    LIST(APPEND plugin_dirswithoption "${CMAKE_CURRENT_SOURCE_DIR}/${plugin_name}^^${option_name}")
  ENDFOREACH()

  ctkFunctionGenerateDGraphInput(${CMAKE_CURRENT_BINARY_DIR} "${plugin_dirswithoption}" WITH_EXTERNALS)
  ctkMacroValidateBuildOptions("${CMAKE_CURRENT_BINARY_DIR}" "${CTK_DGRAPH_EXECUTABLE}" "${plugin_dirswithoption}")

  FOREACH(plugin ${plugin_subdirs})
    IF(BUILD_${plugin})
      ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/${plugin})
    ENDIF()
  ENDFOREACH()

ENDFUNCTION()
