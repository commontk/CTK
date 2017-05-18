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
  set(CTK_QT_VERSION "4" CACHE STRING "Expected Qt version")
  mark_as_advanced(CTK_QT_VERSION)

  set_property(CACHE CTK_QT_VERSION PROPERTY STRINGS 4 5)

  if(NOT (CTK_QT_VERSION VERSION_EQUAL "4" OR CTK_QT_VERSION VERSION_EQUAL "5"))
    message(FATAL_ERROR "Expected value for CTK_QT_VERSION is either '4' or '5'")
  endif()


  if(CTK_QT_VERSION VERSION_GREATER "4")
    cmake_minimum_required(VERSION 2.8.12)
    set(CTK_QT5_COMPONENTS Core Xml XmlPatterns Concurrent Sql Test)
    if(CTK_LIB_Widgets OR CTK_LIB_CommandLineModules/Frontend/QtGui OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      list(APPEND CTK_QT5_COMPONENTS Widgets OpenGL UiTools)
    endif()
    if(CTK_LIB_CommandLineModules/Frontend/QtWebKit OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      list(APPEND CTK_QT5_COMPONENTS WebKitWidgets)
    endif()
    if(CTK_LIB_XNAT/Core OR CTK_BUILD_ALL OR CTK_BUILD_ALL_LIBRARIES)
      list(APPEND CTK_QT5_COMPONENTS Script)
    endif()
    find_package(Qt5 COMPONENTS ${CTK_QT5_COMPONENTS} REQUIRED)
  else()
    set(minimum_required_qt_version "4.6")

    find_package(Qt4)

    if(QT4_FOUND)

      if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
        message(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
      endif()

      set(QT_USE_QTNETWORK ON)
      set(QT_USE_QTSQL ON)
      set(QT_USE_QTOPENGL ON)
      set(QT_USE_QTXML ON)
      set(QT_USE_QTXMLPATTERNS ON)
      set(QT_USE_QTTEST ${BUILD_TESTING})
      include(${QT_USE_FILE})

      # Set variable QT_INSTALLED_LIBRARY_DIR that will contains
      # Qt shared library
      set(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
      if(WIN32)
        get_filename_component(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
      endif()

    else()
      message(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
    endif()
  endif()

  mark_as_superbuild(CTK_QT_VERSION)
  mark_as_superbuild(QT_QMAKE_EXECUTABLE) # Qt 4

endmacro()
