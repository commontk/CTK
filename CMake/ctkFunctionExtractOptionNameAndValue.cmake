
#
#
#

#! \ingroup CMakeUtilities
function(ctkFunctionExtractOptionNameAndValue my_opt var_opt_name var_opt_value)

 # Make sure option is correctly formated
  if(NOT "${my_opt}" MATCHES "^[- :/A-Za-z0-9._]+:(ON|OFF)$")
    message(FATAL_ERROR "Option ${my_opt} is incorrect. Options should be specified using the following format OPT1:[ON|OFF]. For example OPT1:OFF or OPT2:ON")
  endif()

  # Extract option name and option default value
  string(REGEX REPLACE ":(ON|OFF)$" "\\\\;\\1" my_opt_list ${my_opt})
  set(my_opt_list ${my_opt_list})
  list(GET my_opt_list 0 opt_name)
  list(GET my_opt_list 1 opt_value)

  set(${var_opt_name} ${opt_name} PARENT_SCOPE)
  set(${var_opt_value} ${opt_value} PARENT_SCOPE)
endfunction()

#
# Test - Use cmake -DMACRO_TESTING:BOOL=ON -P ctkFunctionExtractOptionNameAndValue.cmake
#
if(MACRO_TESTING)

  message("Testing ctkFunctionExtractOptionNameAndValue ...")
  #
  # Test1
  #
  set(test1 "john:ON")
  ctkFunctionExtractOptionNameAndValue(${test1} test1_name test1_value)
  
  if(NOT test1_name STREQUAL "john")
    message(FATAL_ERROR "test1_name:${test1_name} - Expected:john")
  endif()
  
  if(NOT test1_value STREQUAL "ON")
    message(FATAL_ERROR "test1_value:${test1_value} - Expected:ON")
  endif()

  #
  # Test2
  #
  set(test2 "doe/john:OFF")
  ctkFunctionExtractOptionNameAndValue(${test2} test2_name test2_value)
  
  if(NOT test2_name STREQUAL "doe/john")
    message(FATAL_ERROR "test1_name:${test2_name} - Expected:doe/john")
  endif()
  
  if(NOT test2_value STREQUAL "OFF")
    message(FATAL_ERROR "test2_value:${test2_value} - Expected:OFF")
  endif()
  
endif()
