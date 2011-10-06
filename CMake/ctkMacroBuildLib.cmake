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
#  CTK/CMake/ctkMacroParseArguments.cmake
#

#! \ingroup CMakeAPI
MACRO(ctkMacroBuildLib)
  ctkMacroParseArguments(MY
    "NAME;EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES;LIBRARY_TYPE"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_NAME)
    MESSAGE(FATAL_ERROR "NAME is mandatory")
  ENDIF()
  STRING(REGEX MATCH "^CTK.+" valid_library_name ${MY_NAME})
  IF(NOT valid_library_name)
    MESSAGE(FATAL_ERROR "CTK library name [${MY_NAME}] should start with 'CTK' uppercase !")
  ENDIF()
  IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
    MESSAGE(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_LIBRARY_TYPE)
    SET(MY_LIBRARY_TYPE "SHARED")
  ENDIF()

  # Define library name
  SET(lib_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs
  SET(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    # with CMake >2.9, use QT4_MAKE_OUTPUT_FILE instead ?
    ${CMAKE_CURRENT_BINARY_DIR}/Resources/UI
    ${MY_INCLUDE_DIRECTORIES}
    )

  # Add the include directories from the library dependencies
  ctkFunctionGetIncludeDirs(my_includes ${lib_name})

  INCLUDE_DIRECTORIES(
    ${my_includes}
    )

  # Add the library directories from the external project
  ctkFunctionGetLibraryDirs(my_library_dirs ${lib_name})

  LINK_DIRECTORIES(
    ${my_library_dirs}
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
  IF(MY_MOC_SRCS)
    # this is a workaround for Visual Studio. The relative include paths in the generated
    # moc files can get very long and can't be resolved by the MSVC compiler.
    FOREACH(moc_src ${MY_MOC_SRCS})
      QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src})
    ENDFOREACH()
  ENDIF()
  QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  IF(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MOC_CPP_DECORATOR}
    )

  ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Set labels associated with the target.
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES LABELS ${lib_name})

  # Apply user-defined properties to the library target.
  IF(CTK_LIBRARY_PROPERTIES AND MY_LIBRARY_TYPE STREQUAL "SHARED")
    SET_TARGET_PROPERTIES(${lib_name} PROPERTIES ${CTK_LIBRARY_PROPERTIES})
  ENDIF()
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES CTK_LIB_TARGET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  # Library properties specific to STATIC build
  IF(MY_LIBRARY_TYPE STREQUAL "STATIC")
    IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      SET_TARGET_PROPERTIES(${lib_name} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF()
  ENDIF()

  # Install rules
  IF(MY_LIBRARY_TYPE STREQUAL "SHARED")
    INSTALL(TARGETS ${lib_name}
      LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Development)
  ENDIF()

  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  IF(MINGW)
    LIST(APPEND my_libs ssp) # add stack smash protection lib
  ENDIF(MINGW)
  TARGET_LINK_LIBRARIES(${lib_name} ${my_libs})

  # Update CTK_BASE_LIBRARIES
  SET(CTK_BASE_LIBRARIES ${my_libs} ${lib_name} CACHE INTERNAL "CTK base libraries" FORCE)
  SET(CTK_LIBRARIES ${CTK_LIBRARIES} ${lib_name} CACHE INTERNAL "CTK libraries" FORCE)
  SET(CTK_BASE_INCLUDE_DIRS ${CTK_BASE_INCLUDE_DIRS} ${my_includes} CACHE INTERNAL "CTK includes" FORCE)

  # Install headers
  FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  INSTALL(FILES
    ${headers}
    ${dynamicHeaders}
    DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
    )

ENDMACRO()


