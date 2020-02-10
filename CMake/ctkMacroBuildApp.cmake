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
macro(ctkMacroBuildApp)
  ctkMacroParseArguments(MY
    "NAME;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES"
    "INSTALL"
    ${ARGN}
    )

  # Keep parameter 'INCLUDE_DIRECTORIES' for backward compatiblity

  # Sanity checks
  if(NOT DEFINED MY_NAME)
    message(FATAL_ERROR "NAME is mandatory")
  endif()

  # Make sure either the source or the binary directory associated with the application
  # contains a file named ${MY_NAME}Main.cpp
  set(expected_mainfile ${MY_NAME}Main.cpp)
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile} AND
      NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${expected_mainfile}.in AND
      NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${expected_mainfile})
    message(FATAL_ERROR "Application directory: ${MY_NAME} should contain"
                        " a file named ${expected_mainfile} or ${expected_mainfile}.in")
  endif()

  # Define library name
  set(proj_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs
  set(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  # Add the include directories from the library dependencies
  ctkFunctionGetIncludeDirs(my_includes ${proj_name})

  include_directories(${my_includes})

  # Add the library directories from the external project
  ctkFunctionGetLibraryDirs(my_library_dirs ${proj_name})

  link_directories(
    ${my_library_dirs}
    )

  if(CTK_QT_VERSION VERSION_LESS "5")
    # Add Qt include dirs and defines
    include(${QT_USE_FILE})
  endif()

  # Make sure variable are cleared
  set(MY_UI_CPP)
  set(MY_MOC_CPP)
  set(MY_QRC_SRCS)

  if (CTK_QT_VERSION VERSION_GREATER "4")
    # Wrap
    if(MY_MOC_SRCS)
      # this is a workaround for Visual Studio. The relative include paths in the generated
      # moc files can get very long and can't be resolved by the MSVC compiler.
      foreach(moc_src ${MY_MOC_SRCS})
        QT5_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src} OPTIONS -DHAVE_QT5)
      endforeach()
    endif()
    QT5_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
    if(DEFINED MY_RESOURCES)
      QT5_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  else()
    # Wrap
    if(MY_MOC_SRCS)
      # this is a workaround for Visual Studio. The relative include paths in the generated
      # moc files can get very long and can't be resolved by the MSVC compiler.
      foreach(moc_src ${MY_MOC_SRCS})
        QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src})
      endforeach()
    endif()
    QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
    if(DEFINED MY_RESOURCES)
      QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  endif()

  source_group("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  source_group("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    )

  # Create executable
  ctk_add_executable_utf8(${proj_name}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Set labels associated with the target.
  set_target_properties(${proj_name} PROPERTIES LABELS ${proj_name})

  # Install rules
  install(TARGETS ${proj_name}
    RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR} COMPONENT RuntimeApplications
    )

  set(my_libs
    ${MY_TARGET_LIBRARIES}
    )
  target_link_libraries(${proj_name} ${my_libs})

  # Install headers
  if(MY_INSTALL)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES
      ${headers}
      ${dynamicHeaders}
      DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
      )
  endif()

endmacro()
