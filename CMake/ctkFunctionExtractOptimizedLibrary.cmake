#!
#! Extracts the optimized library filepath from a list in the target_link_libraries()
#! format, e. g. "optimized;release.lib;debug;debug.lib".
#!
#! If the list is a single filepath, return it instead.
#!
#! \param lib_list A target_link_libraries() library list or a single filepath.
#! \param var_optimized_lib A variable name containing the output.
#!
function(ctkFunctionExtractOptimizedLibrary lib_list var_optimized_lib)

  list(LENGTH ${lib_list} lib_list_length)
  if(lib_list_length EQUAL 1)
    set(${var_optimized_lib} "${${lib_list}}" PARENT_SCOPE)
    return()
  endif()

  list(FIND ${lib_list} "optimized" optimized_keyword_index)
  if(optimized_keyword_index EQUAL -1)
    message(FATAL_ERROR "List \"${${lib_list}}\" does not contain \"optimized\" keyword")
  endif()

  math(EXPR last_lib_list_index "${lib_list_length} - 1")
  if(NOT optimized_keyword_index LESS last_lib_list_index)
   message(FATAL_ERROR "List \"${${lib_list}}\" ended unexpectedly")
  endif()

  math(EXPR optimized_lib_index "${optimized_keyword_index} + 1")
  list(GET ${lib_list} ${optimized_lib_index} optimized_lib)
  set(${var_optimized_lib} "${optimized_lib}" PARENT_SCOPE)

endfunction()
