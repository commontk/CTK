cmake_minimum_required(VERSION 2.8.7)

project(LibB NONE)

function(check_variable var_name expected_value)
  if(NOT "x${${var_name}}" STREQUAL "x${expected_value}")
    message(FATAL_ERROR "Problem with mark_as_superbuild() - CMake variable [${var_name}] is incorrectly set !\n"
                        "current:${${var_name}}\n"
                        "expected:${expected_value}")
  endif()
endfunction()

check_variable(LibB_VAR "LibB")

check_variable(MULTIPROJECT_VAR "MultiProjectVar")

check_variable(LibB_EP_LABEL_MULTIPROJECT "MULTIPROJECT_VAR_WITH_LABELS")
check_variable(MULTIPROJECT_VAR_WITH_LABELS "MultiProjectVarWithLabels")

check_variable(ALL_PROJECTS_VAR "AllProjectsVar")