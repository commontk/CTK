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

#! \ingroup CMakeAPI
MACRO(ctkMacroSetupExternalPlugins )

  ctkMacroParseArguments(MY "BUILD_OPTION_PREFIX;BUILD_ALL" "COMPACT_OPTIONS" ${ARGN})

  IF(NOT MY_DEFAULT_ARGS)
    MESSAGE(FATAL_ERROR "Empty plugin list")
  ENDIF()

  SET(plugin_list ${MY_DEFAULT_ARGS})

  IF(NOT MY_BUILD_OPTION_PREFIX)
    SET(MY_BUILD_OPTION_PREFIX "BUILD_")
  ENDIF()

  IF(NOT MY_BUILD_ALL)
    SET(MY_BUILD_ALL 0)
  ENDIF()

  # Set up Qt, if not already done
  IF(NOT QT4_FOUND)
    SET(minimum_required_qt_version "4.6")
    FIND_PACKAGE(Qt4 REQUIRED)

    IF("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
      MESSAGE(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    ENDIF()
  ENDIF()
  
  # Set variable QT_INSTALLED_LIBRARY_DIR that will contains
  # Qt shared library
  SET(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
  IF(WIN32)
    GET_FILENAME_COMPONENT(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
  ENDIF()

  SET(plugin_dirswithoption )
  SET(plugin_subdirs )
  FOREACH(plugin ${plugin_list})
    ctkFunctionExtractOptionNameAndValue(${plugin} plugin_name plugin_value)
    IF(MY_COMPACT_OPTIONS)
      STRING(REPLACE "/" ";" _tokens ${plugin_name})
      LIST(GET _tokens -1 option_name)
      SET(option_name ${MY_BUILD_OPTION_PREFIX}${option_name})
    ELSE()
      SET(option_name ${MY_BUILD_OPTION_PREFIX}${plugin_name})
    ENDIF()
    SET(${plugin_name}_option_name ${option_name})

    OPTION(${option_name} "Build ${plugin_name} Plugin." ${plugin_value})
    IF(MY_BUILD_ALL)
      SET(${option_name} 1)
    ENDIF()

    LIST(APPEND plugin_subdirs "${plugin_name}")
    LIST(APPEND plugin_dirswithoption "${CMAKE_CURRENT_SOURCE_DIR}/${plugin_name}^^${option_name}")
  ENDFOREACH()

  ctkFunctionGenerateDGraphInput(${CMAKE_CURRENT_BINARY_DIR} "${plugin_dirswithoption}" WITH_EXTERNALS)
  ctkMacroValidateBuildOptions("${CMAKE_CURRENT_BINARY_DIR}" "${CTK_DGRAPH_EXECUTABLE}" "${plugin_dirswithoption}")

  # Get the gcc version (GCC_VERSION will be empty if the compiler is not gcc).
  # This will be used in the ctkMacroBuildPlugin macro to conditionally set compiler flags.
  ctkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)

  FOREACH(plugin ${plugin_subdirs})
    IF(${${plugin}_option_name})
      ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/${plugin})
    ENDIF()
  ENDFOREACH()

ENDMACRO()
