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

#
# Depends on:
#  cmake_parse_arguments ( >= CMake 2.8.3)
#

#! \ingroup CMakeAPI
macro(ctkMacroBuildQtPlugin)
  cmake_parse_arguments(MY
    "" # no options
    "NAME;EXPORT_DIRECTIVE;FOLDER;PLUGIN_DIR" # one value args
    "SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES" # multi value args
    ${ARGN}
    )

  # Sanity checks
  if(NOT DEFINED MY_NAME)
    message(FATAL_ERROR "NAME is mandatory")
  endif()
  if(NOT DEFINED MY_EXPORT_DIRECTIVE)
    message(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  endif()
  if (NOT DEFINED MY_PLUGIN_DIR)
    message(FATAL_ERROR "PLUGIN_DIR (e.g. designer, iconengines, imageformats...) is mandatory")
  endif()
  set(MY_LIBRARY_TYPE "MODULE")

  # Define library name
  set(lib_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs

  set(my_includes
    ${QT_QTDESIGNER_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MY_INCLUDE_DIRECTORIES}
    )
  if(CTK_SOURCE_DIR)
    # Add the include directories from the library dependencies
    ctkFunctionGetIncludeDirs(my_includes ${MY_TARGET_LIBRARIES})
  endif()
  include_directories(
    ${my_includes}
    )

  set(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  set(MY_EXPORT_HEADER_PREFIX ${MY_NAME})
  string(REGEX REPLACE "^CTK" "ctk" MY_EXPORT_HEADER_PREFIX ${MY_EXPORT_HEADER_PREFIX})
  set(MY_LIBNAME ${lib_name})

  if(NOT CTK_EXPORT_HEADER_TEMPLATE)
    message(FATAL_ERROR "CTK_EXPORT_HEADER_TEMPLATE is mandatory")
  endif()

  configure_file(
    ${CTK_EXPORT_HEADER_TEMPLATE}
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  set(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  set(MY_MOC_CPP)
  set(MY_UI_CPP)
  set(MY_QRC_SRCS)

  # Wrap
  set(MY_QRC_SRCS "")
  if(CTK_QT_VERSION VERSION_GREATER "4")
    qt5_wrap_cpp(MY_MOC_CPP ${MY_MOC_SRCS} TARGET ${lib_name} OPTIONS -DHAVE_QT5)
    if(DEFINED MY_RESOURCES)
      qt5_add_resources(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  else()
    QT4_WRAP_CPP(MY_MOC_CPP ${MY_MOC_SRCS})
    if(DEFINED MY_RESOURCES)
      QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  endif()

  if(CTK_QT_VERSION VERSION_GREATER "4")
    if(Qt5Widgets_FOUND)
      qt5_wrap_ui(MY_UI_CPP ${MY_UI_FORMS})
    elseif(MY_UI_FORMS)
      message(WARNING "Argument UI_FORMS ignored because Qt5Widgets module was not specified")
    endif()
  else()
    QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  endif()

  source_group("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  source_group("Generated" FILES
    ${MY_MOC_CPP}
    ${MY_QRC_SRCS}
    ${MY_UI_CPP}
    )

  add_library(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Extract library name associated with the plugin and use it as label
  string(REGEX REPLACE "(.*)Plugin[s]?" "\\1" label ${lib_name})

  # Apply properties to the library target.
  set_target_properties(${lib_name}  PROPERTIES
    COMPILE_FLAGS "-DQT_PLUGIN"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${MY_PLUGIN_DIR}"
    LABELS ${label}
    )

  set(my_libs
    ${MY_TARGET_LIBRARIES}
    ${QT_QTDESIGNER_LIBRARY}
    )
  target_link_libraries(${lib_name} ${my_libs})

  if(NOT "${MY_FOLDER}" STREQUAL "")
    set_target_properties(${lib_name} PROPERTIES FOLDER ${MY_FOLDER})
  endif()

  # Install the library
  # CTK_INSTALL_QTPLUGIN_DIR:STRING can be passed when configuring CTK
  # By default, it is the same path as CTK_INSTALL_LIB_DIR
  # Plugins are installed in a subdirectory corresponding to their types (e.g. designer, iconengines, imageformats...)
  install(TARGETS ${lib_name}
    RUNTIME DESTINATION ${CTK_INSTALL_QTPLUGIN_DIR}/${MY_PLUGIN_DIR} COMPONENT RuntimePlugins
    LIBRARY DESTINATION ${CTK_INSTALL_QTPLUGIN_DIR}/${MY_PLUGIN_DIR} COMPONENT RuntimePlugins
    ARCHIVE DESTINATION ${CTK_INSTALL_QTPLUGIN_DIR}/${MY_PLUGIN_DIR} COMPONENT Development
    )

  # Install headers - Are headers required ?
  #file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  #install(FILES
  #  ${headers}
  #  DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  #  )


  # Since Qt expects plugins to be directly located under the
  # subdirectory (e.g. 'designer') but not deeper (e.g. designer/Debug), let's copy them.

  if(NOT CMAKE_CFG_INTDIR STREQUAL ".")
    get_target_property(DIR_PATH ${lib_name} LIBRARY_OUTPUT_DIRECTORY)

    add_custom_command(
      TARGET ${lib_name}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_name}> ${DIR_PATH}/../${MY_PLUGIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${lib_name}${CMAKE_BUILD_TYPE}${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  endif()

endmacro()

macro(ctkMacroBuildQtDesignerPlugin)
  ctkMacroBuildQtPlugin(
    PLUGIN_DIR designer
    ${ARGN})
  if(CTK_QT_VERSION VERSION_GREATER "4")
    find_package(Qt5Designer REQUIRED)
    target_link_libraries(${lib_name} Qt5::Designer)
  endif()
endmacro()

macro(ctkMacroBuildQtIconEnginesPlugin)
  ctkMacroBuildQtPlugin(
    PLUGIN_DIR iconengines
    ${ARGN})
endmacro()

macro(ctkMacroBuildQtStylesPlugin)
  ctkMacroBuildQtPlugin(
    PLUGIN_DIR styles
    ${ARGN})
endmacro()

