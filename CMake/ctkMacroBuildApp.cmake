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
MACRO(ctkMacroBuildApp)
  ctkMacroParseArguments(MY
    "NAME;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_NAME)
    MESSAGE(FATAL_ERROR "NAME is mandatory")
  ENDIF()

  # Make sure either the source or the binary directory associated with the application
  # contains a file named ${MY_NAME}Main.cpp
  set(expected_mainfile ${MY_NAME}Main.cpp)
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile} AND
      NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile}.in AND
      NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${expected_mainfile})
    MESSAGE(FATAL_ERROR "Application directory: ${MY_NAME} should contain"
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
  ctkFunctionGetIncludeDirs(my_includes ${proj_name})

  INCLUDE_DIRECTORIES(${my_includes})

  # Add the library directories from the external project
  ctkFunctionGetLibraryDirs(my_library_dirs ${proj_name})

  LINK_DIRECTORIES(
    ${my_library_dirs}
    )

  # Make sure variable are cleared
  SET(MY_UI_CPP)
  SET(MY_MOC_CPP)
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
    )

  # Create executable
  ADD_EXECUTABLE(${proj_name}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Set labels associated with the target.
  SET_TARGET_PROPERTIES(${proj_name} PROPERTIES LABELS ${proj_name})

  # Install rules
  INSTALL(TARGETS ${proj_name}
    RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR} COMPONENT RuntimeApplications
    )

  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )
  TARGET_LINK_LIBRARIES(${proj_name} ${my_libs})

  # Install headers
  FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  INSTALL(FILES
    ${headers}
    ${dynamicHeaders}
    DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
    )

ENDMACRO()


