###########################################################################
#
#  Library:   CTK
# 
#  Copyright (c) 2010  Kitware Inc.
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

MACRO(ctkMacroBuildApp)
  ctkMacroParseArguments(MY
    "NAME;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_NAME)
    MESSAGE(SEND_ERROR "NAME is mandatory")
  ENDIF()
#   IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
#     MESSAGE(SEND_ERROR "EXPORT_DIRECTIVE is mandatory")
#   ENDIF()
#   IF(NOT DEFINED MY_LIBRARY_TYPE)
#     SET(MY_LIBRARY_TYPE "SHARED")
#   ENDIF()

  # Make sure either the source or the binary directory associated with the application
  # contains a file named ${MY_NAME}Main.cpp
  set(expected_mainfile ${MY_NAME}Main.cpp)
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile} AND
      NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile}.in AND
      NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${expected_mainfile})
    MESSAGE(FATAL_ERROR "Application directory: ${MY_NAME} should contains"
                        " a file named ${expected_mainfile} or ${expected_mainfile}.in")
  endif()

  # Define library name
  SET(proj_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs
  SET(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MY_INCLUDE_DIRECTORIES}
    )  

  # Add the include directories from the library dependencies
  # The variable ${lib_name}_DEPENDENCIES is set in the
  # macro ctkMacroValidateBuildOptions
  SET(ctk_deps )
  SET(ext_deps )
  ctkMacroGetAllCTKTargetLibraries("${${proj_name}_DEPENDENCIES}" ctk_deps)
  ctkMacroGetAllNonCTKTargetLibraries("${${proj_name}_DEPENDENCIES}" ext_deps)

  FOREACH(dep ${ctk_deps})
    LIST(APPEND my_includes
         ${${dep}_SOURCE_DIR}
         ${${dep}_BINARY_DIR}
         )
  ENDFOREACH()

  FOREACH(dep ${ext_deps})
    LIST(APPEND my_includes ${${dep}_INCLUDE_DIRS})
  ENDFOREACH()

  LIST(REMOVE_DUPLICATES my_includes)

  INCLUDE_DIRECTORIES(${my_includes})

#   SET(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
#   SET(MY_EXPORT_HEADER_PREFIX ${MY_NAME})
#   SET(MY_LIBNAME ${lib_name})
  
#   CONFIGURE_FILE(
#     ${CTK_SOURCE_DIR}/Libs/CTKExport.h.in
#     ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
#     )
#   SET(dynamicHeaders
#     "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  SET(MY_UI_CXX)
  SET(MY_MOC_CXX)
  SET(MY_QRC_SRCS)

  # Wrap
  QT4_WRAP_CPP(MY_MOC_CXX ${MY_MOC_SRCS})
  QT4_WRAP_UI(MY_UI_CXX ${MY_UI_FORMS})
  IF(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    )

  # Create executable
  ADD_EXECUTABLE(${proj_name}
    ${MY_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    ${MY_QRC_SRCS}
    )
#   ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
#     ${MY_SRCS}
#     ${MY_MOC_CXX}
#     ${MY_UI_CXX}
#     ${MY_QRC_SRCS}
#     )

  # Set labels associated with the target.
  SET_TARGET_PROPERTIES(${proj_name} PROPERTIES LABELS ${proj_name})
  
  # Install rules
  IF(CTK_BUILD_SHARED_LIBS)
    INSTALL(TARGETS ${proj_name}
      RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR} COMPONENT Runtime
      LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Runtime
      ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Development)
  ENDIF()

  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )
  TARGET_LINK_LIBRARIES(${proj_name} ${my_libs})

  # Update CTK_BASE_LIBRARIES
#   SET(CTK_BASE_LIBRARIES ${my_libs} ${lib_name} CACHE INTERNAL "CTK base libraries" FORCE)
  
  # Install headers
  FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  INSTALL(FILES
    ${headers}
    ${dynamicHeaders}
    DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
    )

ENDMACRO()


