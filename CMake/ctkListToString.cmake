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

function(ctk_list_to_string separator input_list output_string_var)
  set(_string "")
  cmake_policy(PUSH)
  cmake_policy(SET CMP0007 OLD)
  # Get list length
  list(LENGTH input_list list_length)
  # If the list has 0 or 1 element, there is no need to loop over.
  if(list_length LESS 2)
    set(_string  "${input_list}")
  else()
    math(EXPR last_element_index "${list_length} - 1")
    foreach(index RANGE ${last_element_index})
      # Get current item_value
      list(GET input_list ${index} item_value)
      # .. and append to output string
      set(_string  "${_string}${item_value}")
      # Append separator if current element is NOT the last one.
      if(NOT index EQUAL last_element_index)
        set(_string  "${_string}${separator}")
      endif()
    endforeach()
  endif()
  set(${output_string_var} ${_string} PARENT_SCOPE)
  cmake_policy(POP)
endfunction()


#
# cmake -DTEST_ctk_list_to_string_test:BOOL=ON -P ListToString.cmake
#
function(ctk_list_to_string_test)

  function(ctk_list_to_string_test_check id current_output expected_output)
    if(NOT "${current_output}" STREQUAL "${expected_output}")
      message(FATAL_ERROR "Problem with ctk_list_to_string() - See testcase: ${id}\n"
                          "current_output:${current_output}\n"
                          "expected_output:${expected_output}")
    endif()
  endfunction()

  set(id 1)
  set(case${id}_input "")
  set(case${id}_expected_output "")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 2)
  set(case${id}_input item1)
  set(case${id}_expected_output "item1")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 3)
  set(case${id}_input item1 item2)
  set(case${id}_expected_output "item1^^item2")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 4)
  set(case${id}_input item1 item2 item3)
  set(case${id}_expected_output "item1^^item2^^item3")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 5)
  set(case${id}_input item1 item2 item3 item4)
  set(case${id}_expected_output "item1^^item2^^item3^^item4")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 6)
  set(case${id}_input item1 "" item3 item4)
  set(case${id}_expected_output "item1^^item3^^item4")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 7)
  set(case${id}_input item1 ^^item2 item3 item4)
  set(case${id}_expected_output "item1^^^^item2^^item3^^item4")
  ctk_list_to_string("^^" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 8)
  set(case${id}_input item1 item2 item3 item4)
  set(case${id}_expected_output "item1item2item3item4")
  ctk_list_to_string("" "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  set(id 9)
  set(case${id}_input item1 item2 item3 item4)
  set(case${id}_expected_output "item1 item2 item3 item4")
  ctk_list_to_string(" " "${case${id}_input}" case${id}_current_output)
  ctk_list_to_string_test_check(${id} "${case${id}_current_output}" "${case${id}_expected_output}")

  message("SUCCESS")
endfunction()
if(TEST_ctk_list_to_string_test)
  ctk_list_to_string_test()
endif()
