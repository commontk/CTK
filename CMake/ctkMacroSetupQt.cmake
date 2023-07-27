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

#! \ingroup CMakeUtilities
macro(ctkMacroSetupQt)
  set(CTK_QT_VERSION "5" CACHE STRING "Expected Qt version")
  mark_as_advanced(CTK_QT_VERSION)

  set_property(CACHE CTK_QT_VERSION PROPERTY STRINGS 5)

  if(NOT CTK_QT_VERSION VERSION_EQUAL "5")
    message(FATAL_ERROR "Expected value for CTK_QT_VERSION is '5'")
  endif()

  if(CTK_QT_VERSION VERSION_EQUAL "5")
    cmake_minimum_required(VERSION 2.8.12)
    find_package(Qt5 COMPONENTS Core)
    set(CTK_QT5_COMPONENTS Core Xml XmlPatterns Concurrent Sql Test Multimedia)
    if(CTK_ENABLE_Widgets OR CTK_LIB_Widgets OR CTK_LIB_CommandLineModules/Frontend/QtGui OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      list(APPEND CTK_QT5_COMPONENTS Widgets OpenGL UiTools)
    endif()
    if(CTK_LIB_CommandLineModules/Frontend/QtWebKit OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      if(TARGET Qt5::WebKitWidgets)
        list(APPEND CTK_QT5_COMPONENTS WebKitWidgets)
      else()
        list(APPEND CTK_QT5_COMPONENTS WebEngineWidgets)
      endif()
    endif()
    if(CTK_LIB_XNAT/Core OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      list(APPEND CTK_QT5_COMPONENTS Script)
    endif()
    find_package(Qt5 COMPONENTS ${CTK_QT5_COMPONENTS} REQUIRED)

    mark_as_superbuild(Qt5_DIR) # Qt 5

    # XXX Backward compatible way
    if(DEFINED CMAKE_PREFIX_PATH)
      mark_as_superbuild(CMAKE_PREFIX_PATH) # Qt 5
    endif()

    set(_major ${Qt5_VERSION_MAJOR})
    set(_minor ${Qt5_VERSION_MINOR})
    set(_patch ${Qt5_VERSION_PATCH})

    ctk_list_to_string(", " "${CTK_QT5_COMPONENTS}" comma_separated_module_list)
    message(STATUS "Configuring CTK with Qt ${_major}.${_minor}.${_patch} (using modules: ${comma_separated_module_list})")

  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()

  mark_as_superbuild(CTK_QT_VERSION)
endmacro()
