cmake_minimum_required(VERSION 2.8.7)

project(LibA NONE)

function(check_variable var_name expected_value)
  if(NOT "x${${var_name}}" STREQUAL "x${expected_value}")
    message(FATAL_ERROR "Problem with mark_as_superbuild() - CMake variable [${var_name}] is incorrectly set !\n"
                        "current:${${var_name}}\n"
                        "expected:${expected_value}")
  endif()
endfunction()

check_variable(LibA_VAR "LibA")

check_variable(LibA_EP_LABEL_LABEL_A "LibA_VAR_WITH_LABELS")