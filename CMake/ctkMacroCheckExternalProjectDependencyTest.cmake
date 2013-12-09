
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})
include(ctkMacroCheckExternalProjectDependency)

#
# cmake -DTEST__sb_cmakevar_to_cmakearg_test:BOOL=ON -P ctkMacroCheckExternalProjectDependencyTest.cmake
#
function(_sb_cmakevar_to_cmakearg_test)
  function(check_test_result id current_output expected_output
      current_varname expected_varname
      current_vartype expected_vartype)
    if(NOT "${current_output}" STREQUAL "${expected_output}")
      message(FATAL_ERROR "Problem with _sb_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_output:${current_output}\n"
                          "expected_output:${expected_output}")
    endif()
    if(NOT "${current_varname}" STREQUAL "${expected_varname}")
      message(FATAL_ERROR "Problem with _sb_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_varname:${current_varname}\n"
                          "expected_varname:${expected_varname}")
    endif()
    if(NOT "${current_vartype}" STREQUAL "${expected_vartype}")
      message(FATAL_ERROR "Problem with _sb_cmakevar_to_cmakearg() - See testcase: ${id}\n"
                          "current_vartype:${current_vartype}\n"
                          "expected_vartype:${expected_vartype}")
    endif()
  endfunction()

  set(id 1)
  set(case${id}_input "Hello")
  set(case${id}_expected_varname "")
  set(case${id}_expected_vartype "")
  set(case${id}_expected_cmake_arg_var "-Dcase${id}_input:STRING=${case${id}_input}")
  _sb_cmakevar_to_cmakearg("case${id}_input:STRING"
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
  _sb_cmakevar_to_cmakearg("case${id}_input:STRING"
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
  _sb_cmakevar_to_cmakearg("case${id}_input:STRING"
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
  _sb_cmakevar_to_cmakearg("case${id}_input:STRING"
      case${id}_cmake_arg_var CMAKE_CMD
      case${id}_varname case${id}_vartype)
  check_test_result(${id}
      "${case${id}_cmake_arg_var}" "${case${id}_expected_cmake_arg_var}"
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  message("SUCCESS")
endfunction()
if(TEST__sb_cmakevar_to_cmakearg_test)
  _sb_cmakevar_to_cmakearg_test()
endif()

#
# cmake -DTEST__sb_extract_varname_and_vartype_test:BOOL=ON -P ctkMacroCheckExternalProjectDependencyTest.cmake
#
function(_sb_extract_varname_and_vartype_test)
  function(check_test_result id current_varname expected_varname
      current_vartype expected_vartype)
    if(NOT "${current_varname}" STREQUAL "${expected_varname}")
      message(FATAL_ERROR "Problem with _sb_extract_varname_and_vartype() - See testcase: ${id}\n"
                          "current_varname:${current_varname}\n"
                          "expected_varname:${expected_varname}")
    endif()
    if(NOT "${current_vartype}" STREQUAL "${expected_vartype}")
      message(FATAL_ERROR "Problem with _sb_extract_varname_and_vartype() - See testcase: ${id}\n"
                          "current_vartype:${current_vartype}\n"
                          "expected_vartype:${expected_vartype}")
    endif()
  endfunction()

  set(id 1)
  set(case${id}_input "VAR:STRING")
  set(case${id}_expected_varname "VAR")
  set(case${id}_expected_vartype "STRING")
  _sb_extract_varname_and_vartype("${case${id}_input}"
      case${id}_varname case${id}_vartype)
  check_test_result(${id}
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  set(id 2)
  set(case${id}_input "VAR:STRING")
  set(case${id}_expected_varname "VAR")
  set(case${id}_expected_vartype "")
  _sb_extract_varname_and_vartype("${case${id}_input}"
      case${id}_varname)
  check_test_result(${id}
      "${case${id}_varname}" "${case${id}_expected_varname}"
      "${case${id}_vartype}" "${case${id}_expected_vartype}")

  message("SUCCESS")
endfunction()
if(TEST__sb_extract_varname_and_vartype_test)
  _sb_extract_varname_and_vartype_test()
endif()
