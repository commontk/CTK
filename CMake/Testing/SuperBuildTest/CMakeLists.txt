cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ${CMAKE_MODULE_PATH})
set(EXTERNAL_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/TheExternals)
set(EXTERNAL_PROJECT_FILE_PREFIX "TheExternal_")
include(ExternalProject)
include(ctkMacroCheckExternalProjectDependency)

project(SuperBuildTest NONE)

option(${PROJECT_NAME}_SUPERBUILD "Build ${PROJECT_NAME} and the projects it depends" ON)
mark_as_advanced(${PROJECT_NAME}_SUPERBUILD)

if(${PROJECT_NAME}_SUPERBUILD)
  option(FOO_OPTION "Option mark_as_superbuild" ON)
  mark_as_superbuild(FOO_OPTION)

  set(FOO_VAR "FooVar")
  mark_as_superbuild(FOO_VAR)

  set(FOO_LIST Item1 Item2)
  mark_as_superbuild(FOO_LIST)

  set(FOO_LIST_FOR_CMAKE_CMD "Item1${EP_LIST_SEPARATOR}Item2")
  mark_as_superbuild(FOO_LIST_FOR_CMAKE_CMD CMAKE_CMD)

  set(FOO_VAR_WITH_LABELS "FooVarWithLabels")
  mark_as_superbuild(VARS FOO_VAR_WITH_LABELS LABELS "LABEL_A" "LABEL_B")

  set(BAR_VAR_WITH_LABELS "BarVarWithLabels")
  mark_as_superbuild(VARS BAR_VAR_WITH_LABELS LABELS "LABEL_B")

  set(LibA_VAR "LibA")
  mark_as_superbuild(VARS LibA_VAR PROJECTS LibA)

  set(LibB_VAR "LibB")
  mark_as_superbuild(VARS LibB_VAR PROJECTS LibB)

  set(LibA_VAR_WITH_LABELS "LibAWithLabels")
  mark_as_superbuild(VARS LibA_VAR_WITH_LABELS PROJECTS LibA LABELS "LABEL_A")

  set(MULTIPROJECT_VAR "MultiProjectVar")
  mark_as_superbuild(VARS MULTIPROJECT_VAR PROJECTS ${PROJECT_NAME} LibA LibB)

  set(MULTIPROJECT_VAR_WITH_LABELS "MultiProjectVarWithLabels")
  mark_as_superbuild(VARS MULTIPROJECT_VAR_WITH_LABELS PROJECTS ${PROJECT_NAME} LibA LibB LABELS "MULTIPROJECT")

  set(ALL_PROJECTS_VAR "AllProjectsVar")
  mark_as_superbuild(VARS ALL_PROJECTS_VAR ALL_PROJECTS)

  #set(MULTIPROJECT_VAR_WITH_LABELS "MultiProjectVarWithLabels")
  #mark_as_superbuild(VARS MULTIPROJECT_VAR_WITH_LABELS PROJECTS ${PROJECT_NAME} LibA LABELS "MULTIPROJECT")

  # Since LibC depends on LibD, LibD will also be expected to be found on the system
  set(${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibC 1)
endif()

set(${PROJECT_NAME}_DEPENDENCIES LibA LibB LibC)

superbuild_include_dependencies(${PROJECT_NAME})

if(${PROJECT_NAME}_SUPERBUILD)
  ExternalProject_Add(${PROJECT_NAME}
    ${${PROJECT_NAME}_EXTERNAL_PROJECT_ARGS}
    DOWNLOAD_COMMAND ""
    CMAKE_CACHE_ARGS
      -D${PROJECT_NAME}_SUPERBUILD:BOOL=OFF
    SOURCE_DIR ${${PROJECT_NAME}_SOURCE_DIR}
    BINARY_DIR ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}-build
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS
      ${${PROJECT_NAME}_DEPENDENCIES}
    )
  return()
endif()

function(check_variable var_name expected_value)
  if(NOT "x${${var_name}}" STREQUAL "x${expected_value}")
    message(FATAL_ERROR "Problem with mark_as_superbuild() - CMake variable [${var_name}] is incorrectly set !\n"
                        "current:${${var_name}}\n"
                        "expected:${expected_value}")
  endif()
endfunction()

check_variable(FOO_OPTION "ON")
check_variable(FOO_VAR "FooVar")
check_variable(FOO_LIST "Item1;Item2")
check_variable(FOO_LIST_FOR_CMAKE_CMD "Item1;Item2")
check_variable(FOO_VAR_WITH_LABELS "FooVarWithLabels")
check_variable(BAR_VAR_WITH_LABELS "BarVarWithLabels")

check_variable(SuperBuildTest_EP_LABEL_LABEL_A "FOO_VAR_WITH_LABELS")
check_variable(SuperBuildTest_EP_LABEL_LABEL_B "FOO_VAR_WITH_LABELS;BAR_VAR_WITH_LABELS")

check_variable(SuperBuildTest_EP_LABEL_FIND_PACKAGE "LibA_DIR")
get_filename_component(expected_LibA_DIR "${CMAKE_CURRENT_BINARY_DIR}/../LibA-build" REALPATH)
check_variable(LibA_DIR ${expected_LibA_DIR})

check_variable(SuperBuildTest_EP_LABEL_FIND_PACKAGE "LibB_DIR")
get_filename_component(expected_LibB_DIR "${CMAKE_CURRENT_BINARY_DIR}/../LibB-build" REALPATH)
check_variable(LibB_DIR ${expected_LibB_DIR})

check_variable(SuperBuildTest_EP_LABEL_USE_SYSTEM
  "${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibA;${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibB;${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibC;${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibD")
check_variable(${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibA "")
check_variable(${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibB "")
check_variable(${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibC 1)
check_variable(${CMAKE_PROJECT_NAME}_USE_SYSTEM_LibD 1)

check_variable(MULTIPROJECT_VAR "MultiProjectVar")

check_variable(SuperBuildTest_EP_LABEL_MULTIPROJECT "MULTIPROJECT_VAR_WITH_LABELS")
check_variable(MULTIPROJECT_VAR_WITH_LABELS "MultiProjectVarWithLabels")

check_variable(ALL_PROJECTS_VAR "AllProjectsVar")
