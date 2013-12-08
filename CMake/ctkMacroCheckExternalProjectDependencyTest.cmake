
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})
include(ctkMacroCheckExternalProjectDependency)

#
# cmake -DTEST_superbuild_cmakevar_to_cmakearg_test:BOOL=ON -P ctkMacroCheckExternalProjectDependencyTest.cmake
#
function(superbuild_cmakevar_to_cmakearg_test)
  function(check_test_result id current_output expected_output
      current_varname expected_varname
      current_vartype expected_vartype)
    if(NOT "${current_output}" STREQUAL "${expected_output}")
      message(FATAL_ERROR "Problem with superbuild_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_output:${current_output}\n"
                          "expected_output:${expected_output}")
    endif()
    if(NOT "${current_varname}" STREQUAL "${expected_varname}")
      message(FATAL_ERROR "Problem with superbuild_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_varname:${current_varname}\n"
                          "expected_varname:${expected_varname}")
    endif()
    if(NOT "${current_vartype}" STREQUAL "${expected_vartype}")
      message(FATAL_ERROR "Problem with superbuild_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_vartype:${current_vartype}\n"
                          "expected_vartype:${expected_vartype}")
    endif()
  endfunction()

  set(id 1)
  set(case${id}_input "Hello")
  set(case${id}_expected_varname "")
  set(case${id}_expected_vartype "")
  set(case${id}_expected_cmake_arg_var "-Dcase${id}_input:STRING=${case${id}_input}")
  superbuild_cmakevar_to_cmakearg("case${id}_input:STRING"
      case${id}_cmake_arg_var CMAKE_CACHE
      )
  check_test_result(${id}
      "${case${id}_cmake_arg_var}" "${case${id}_expected_cmake_arg_var}"
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  set(id 2)
  set(case${id}_input "Hello")
  set(case${id}_expected_varname "case${id}_input")
  set(case${id}_expected_vartype "STRING")
  set(case${id}_expected_cmake_arg_var "-Dcase${id}_input:STRING=${case${id}_input}")
  superbuild_cmakevar_to_cmakearg("case${id}_input:STRING"
      case${id}_cmake_arg_var CMAKE_CACHE
      case${id}_varname case${id}_vartype)
  check_test_result(${id}
      "${case${id}_cmake_arg_var}" "${case${id}_expected_cmake_arg_var}"
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  set(id 3)
  set(case${id}_input Hello World)
  set(case${id}_expected_varname "case${id}_input")
  set(case${id}_expected_vartype "STRING")
  set(case${id}_expected_cmake_arg_var "-Dcase${id}_input:STRING=${case${id}_input}")
  superbuild_cmakevar_to_cmakearg("case${id}_input:STRING"
      case${id}_cmake_arg_var CMAKE_CACHE
      case${id}_varname case${id}_vartype)
  check_test_result(${id}
      "${case${id}_cmake_arg_var}" "${case${id}_expected_cmake_arg_var}"
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  set(id 4)
  set(case${id}_input Hello World)
  set(case${id}_expected_varname "case${id}_input")
  set(case${id}_expected_vartype "STRING")
  set(case${id}_expected_cmake_arg_var "-Dcase${id}_input:STRING=Hello^^World")
  superbuild_cmakevar_to_cmakearg("case${id}_input:STRING"
      case${id}_cmake_arg_var CMAKE_CMD
      case${id}_varname case${id}_vartype)
  check_test_result(${id}
      "${case${id}_cmake_arg_var}" "${case${id}_expected_cmake_arg_var}"
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  message("SUCCESS")
endfunction()
if(TEST_superbuild_cmakevar_to_cmakearg_test)
  superbuild_cmakevar_to_cmakearg_test()
endif()
