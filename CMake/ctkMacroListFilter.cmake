
# See http://www.cmake.org/Wiki/CMakeMacroListOperations#LIST_FILTER

# LIST_FILTER(<list> <regexp_var> [<regexp_var> ...]
#              [OUTPUT_VARIABLE <variable>])
# Removes items from <list> which do not match any of the specified
# regular expressions. An optional argument OUTPUT_VARIABLE
# specifies a variable in which to store the matched items instead of
# updating <list>
# As regular expressions can not be given to macros (see bug #5389), we pass
# variable names whose content is the regular expressions.
# Note that this macro requires PARSE_ARGUMENTS macro, available here:
# http://www.cmake.org/Wiki/CMakeMacroParseArguments
MACRO(CtkMacroListFilter)
  ctkMacroParseArguments(LIST_FILTER "OUTPUT_VARIABLE" "" ${ARGV})
  # Check arguments.
  LIST(LENGTH LIST_FILTER_DEFAULT_ARGS LIST_FILTER_default_length)
  IF(${LIST_FILTER_default_length} EQUAL 0)
    MESSAGE(FATAL_ERROR "LIST_FILTER: missing list variable.")
  ENDIF(${LIST_FILTER_default_length} EQUAL 0)
  IF(${LIST_FILTER_default_length} EQUAL 1)
    MESSAGE(FATAL_ERROR "LIST_FILTER: missing regular expression variable.")
  ENDIF(${LIST_FILTER_default_length} EQUAL 1)
  # Reset output variable
  IF(NOT LIST_FILTER_OUTPUT_VARIABLE)
    SET(LIST_FILTER_OUTPUT_VARIABLE "LIST_FILTER_internal_output")
  ENDIF(NOT LIST_FILTER_OUTPUT_VARIABLE)
  SET(${LIST_FILTER_OUTPUT_VARIABLE})
  # Extract input list from arguments
  LIST(GET LIST_FILTER_DEFAULT_ARGS 0 LIST_FILTER_input_list)
  LIST(REMOVE_AT LIST_FILTER_DEFAULT_ARGS 0)
  FOREACH(LIST_FILTER_item ${${LIST_FILTER_input_list}})
    FOREACH(LIST_FILTER_regexp_var ${LIST_FILTER_DEFAULT_ARGS})
      FOREACH(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
        IF(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
          LIST(APPEND ${LIST_FILTER_OUTPUT_VARIABLE} ${LIST_FILTER_item})
        ENDIF(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
      ENDFOREACH(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
    ENDFOREACH(LIST_FILTER_regexp_var)
  ENDFOREACH(LIST_FILTER_item)
  # If OUTPUT_VARIABLE is not specified, overwrite the input list.
  IF(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
    SET(${LIST_FILTER_input_list} ${${LIST_FILTER_OUTPUT_VARIABLE}})
  ENDIF(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
ENDMACRO(CtkMacroListFilter)
