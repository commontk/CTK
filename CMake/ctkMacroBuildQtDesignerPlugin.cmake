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
#      http://www.commontk.org/LICENSE
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
#  CTK/CMake/ctkMacroParseArguments.cmake
#

MACRO(ctkMacroBuildQtDesignerPlugin)
  CtkMacroParseArguments(MY
    "NAME;EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES;LIBRARY_TYPE"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_NAME)
    MESSAGE(SEND_ERROR "NAME is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
    MESSAGE(SEND_ERROR "EXPORT_DIRECTIVE is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_LIBRARY_TYPE)
    SET(MY_LIBRARY_TYPE "SHARED")
  ENDIF()

  # Define library name
  SET(lib_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs
  SET(my_includes
    ${CTK_BASE_INCLUDE_DIRS}
    ${QT_QTDESIGNER_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MY_INCLUDE_DIRECTORIES}
    )
  INCLUDE_DIRECTORIES(
    ${my_includes}
    )
 
  SET(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  SET(MY_EXPORT_HEADER_PREFIX ${MY_NAME})
  STRING(REGEX REPLACE "^CTK" "ctk" MY_EXPORT_HEADER_PREFIX ${MY_EXPORT_HEADER_PREFIX})
  SET(MY_LIBNAME ${lib_name})
  
  CONFIGURE_FILE(
    ${CTK_SOURCE_DIR}/Libs/ctkExport.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  SET(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  SET(MY_MOC_CPP)
  SET(MY_UI_CPP)
  SET(MY_QRC_SRCS)

  # Wrap
  QT4_WRAP_CPP(MY_MOC_CPP ${MY_MOC_SRCS})
  QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  SET(MY_QRC_SRCS "")
  IF(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_MOC_CPP}
    ${MY_QRC_SRCS}
    ${MY_UI_CPP}
    )

  ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Apply properties to the library target.
  SET_TARGET_PROPERTIES(${lib_name}  PROPERTIES
    COMPILE_FLAGS "-DQT_PLUGIN"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/designer"
    )
  
  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    ${QT_QTDESIGNER_LIBRARY}
    )
  TARGET_LINK_LIBRARIES(${lib_name} ${my_libs})

  # Install the library
  INSTALL(TARGETS ${lib_name}
    RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR}/designer COMPONENT Runtime
    LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR}/designer COMPONENT Runtime
    ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR}/designer COMPONENT Development)

  # Install headers - Are headers required ?
  #FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  #INSTALL(FILES 
  #  ${headers}
  #  DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  #  )

  
  # Since QtDesigner expects plugin to be directly located under the
  # directory 'designer', let's copy them. 

  IF(NOT CMAKE_CFG_INTDIR STREQUAL ".")
    GET_TARGET_PROPERTY(FILE_PATH ${lib_name} LOCATION)
    GET_TARGET_PROPERTY(DIR_PATH ${lib_name} LIBRARY_OUTPUT_DIRECTORY)
  
    ADD_CUSTOM_COMMAND(
      TARGET ${lib_name}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${FILE_PATH} ${DIR_PATH}/../designer/${CMAKE_SHARED_LIBRARY_PREFIX}${lib_name}${CMAKE_BUILD_TYPE}${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()

ENDMACRO()

