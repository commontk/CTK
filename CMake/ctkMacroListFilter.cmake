
#! See http://www.cmake.org/Wiki/CMakeMacroListOperations#LIST_FILTER
#!
#! Usage:
#! \code
#! ctkMacroListFilter(<list> <regexp_var> [<regexp_var> ...]
#!                    [OUTPUT_VARIABLE <variable>])
#! \endcode
#!
#! Removes items from <list> which do not match any of the specified
#! regular expressions. An optional argument OUTPUT_VARIABLE
#! specifies a variable in which to store the matched items instead of
#! updating <list>
#! As regular expressions can not be given to macros (see bug #5389), we pass
#! variable names whose content is the regular expressions.
#! Note that this macro requires PARSE_ARGUMENTS macro, available here:
#! http://www.cmake.org/Wiki/CMakeMacroParseArguments
#!
#! \ingroup CMakeUtilities
macro(ctkMacroListFilter)
  ctkMacroParseArguments(LIST_FILTER "OUTPUT_VARIABLE" "" ${ARGV})
  # Check arguments.
  list(LENGTH LIST_FILTER_DEFAULT_ARGS LIST_FILTER_default_length)
  if(${LIST_FILTER_default_length} EQUAL 0)
    message(FATAL_ERROR "LIST_FILTER: missing list variable.")
  endif()
  if(${LIST_FILTER_default_length} EQUAL 1)
    message(FATAL_ERROR "LIST_FILTER: missing regular expression variable.")
  endif()
  # Reset output variable
  if(NOT LIST_FILTER_OUTPUT_VARIABLE)
    set(LIST_FILTER_OUTPUT_VARIABLE "LIST_FILTER_internal_output")
  endif()
  set(${LIST_FILTER_OUTPUT_VARIABLE})
  # Extract input list from arguments
  list(GET LIST_FILTER_DEFAULT_ARGS 0 LIST_FILTER_input_list)
  list(REMOVE_AT LIST_FILTER_DEFAULT_ARGS 0)
  foreach(LIST_FILTER_item ${${LIST_FILTER_input_list}})
    foreach(LIST_FILTER_regexp_var ${LIST_FILTER_DEFAULT_ARGS})
      foreach(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
        if(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
          list(APPEND ${LIST_FILTER_OUTPUT_VARIABLE} ${LIST_FILTER_item})
        endif()
      endforeach()
    endforeach()
  endforeach()
  # If OUTPUT_VARIABLE is not specified, overwrite the input list.
  if(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
    set(${LIST_FILTER_input_list} ${${LIST_FILTER_OUTPUT_VARIABLE}})
  endif()
endmacro(ctkMacroListFilter)
