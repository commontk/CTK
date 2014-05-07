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
    set(QT5_INSTALL_PREFIX "" CACHE PATH "The install location of Qt5")
    if(NOT QT5_INSTALL_PREFIX OR NOT EXISTS ${QT5_INSTALL_PREFIX}/bin/qmake)
      message(FATAL_ERROR "You must specify the install location of Qt5")
    endif()
    set(CMAKE_PREFIX_PATH ${QT5_INSTALL_PREFIX})
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

    mark_as_superbuild(QT_QMAKE_EXECUTABLE)

  else()
    message(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
  endif()

endmacro()
