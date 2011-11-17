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
macro(ctkMacroSetupExternalPlugins )

  ctkMacroParseArguments(MY "BUILD_OPTION_PREFIX;BUILD_ALL" "COMPACT_OPTIONS" ${ARGN})

  if(NOT MY_DEFAULT_ARGS)
    message(FATAL_ERROR "Empty plugin list")
  endif()

  set(plugin_list ${MY_DEFAULT_ARGS})

  if(NOT MY_BUILD_OPTION_PREFIX)
    set(MY_BUILD_OPTION_PREFIX "BUILD_")
  endif()

  if(NOT MY_BUILD_ALL)
    set(MY_BUILD_ALL 0)
  endif()

  # Set up Qt, if not already done
  if(NOT QT4_FOUND)
    set(minimum_required_qt_version "4.6")
    find_package(Qt4 REQUIRED)

    if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
      message(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    endif()
  endif()
  
  # Set variable QT_INSTALLED_LIBRARY_DIR that will contains
  # Qt shared library
  set(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
  if(WIN32)
    get_filename_component(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
  endif()

  set(plugin_dirswithoption )
  set(plugin_subdirs )
  foreach(plugin ${plugin_list})
    ctkFunctionExtractOptionNameAndValue(${plugin} plugin_name plugin_value)
    if(MY_COMPACT_OPTIONS)
      string(REPLACE "/" ";" _tokens ${plugin_name})
      list(GET _tokens -1 option_name)
      set(option_name ${MY_BUILD_OPTION_PREFIX}${option_name})
    else()
      set(option_name ${MY_BUILD_OPTION_PREFIX}${plugin_name})
    endif()
    set(${plugin_name}_option_name ${option_name})

    option(${option_name} "Build ${plugin_name} Plugin." ${plugin_value})
    if(MY_BUILD_ALL)
      set(${option_name} 1)
    endif()

    list(APPEND plugin_subdirs "${plugin_name}")
    if(IS_ABSOLUTE ${plugin_name})
      list(APPEND plugin_dirswithoption "${plugin_name}^^${option_name}")
    else()
      list(APPEND plugin_dirswithoption "${CMAKE_CURRENT_SOURCE_DIR}/${plugin_name}^^${option_name}")
    endif()
  endforeach()

  ctkFunctionGenerateDGraphInput(${CMAKE_CURRENT_BINARY_DIR} "${plugin_dirswithoption}" WITH_EXTERNALS)
  ctkMacroValidateBuildOptions("${CMAKE_CURRENT_BINARY_DIR}" "${CTK_DGRAPH_EXECUTABLE}" "${plugin_dirswithoption}")

  # Get the gcc version (GCC_VERSION will be empty if the compiler is not gcc).
  # This will be used in the ctkMacroBuildPlugin macro to conditionally set compiler flags.
  ctkFunctionGetGccversion(${CMAKE_CXX_COMPILER} GCC_VERSION)

  foreach(plugin ${plugin_subdirs})
    if(${${plugin}_option_name})
      if(IS_ABSOLUTE ${plugin})
        # get last directory component
        get_filename_component(_dirname ${plugin} NAME) 
        add_subdirectory(${plugin} private_plugins/${_dirname})
      else()
        add_subdirectory(${plugin})
      endif()
    endif()
  endforeach()

endmacro()
