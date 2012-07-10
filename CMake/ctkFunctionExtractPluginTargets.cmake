#!
#! Extracts target names from a string containing CMake option values.
#!
#! Example usage:
#! \code
#! set(build_options Plugins/org.mydomain.core:ON Plugins/org.mydomain.logic:ON)
#! ctkFunctionExtractPluginTargets("${build_options}" ALL target_names)
#! message("targets: ${target_names}")
#! \endcode
#! will print <code>targets: org_mydomain_core;org_mydomain_logic</code>.
#!
#! \param my_opts A string containing a list of options.
#! \param my_filter One of ON, OFF or ALL. Checks the actual build option of the plugin.
#! \param var_targets A variable name containing the output.
#!
function(ctkFunctionExtractPluginTargets my_opts my_filter var_targets)

  if("${my_filter}" STREQUAL "ON" OR "${my_filter}" STREQUAL "OFF"
     OR "${my_filter}" STREQUAL "ALL")
    set(valid_input 1)
  else()
    set(valid_input 0)
    set(error_msg "${my_filter} is not one of ON, OFF or ALL")
  endif()

  if(NOT valid_input)
    message(FATAL_ERROR "${error_msg}")
  endif()

  set(plugin_targets )
  foreach(opt ${my_opts})
    ctkFunctionExtractOptionNameAndValue(${opt} plugin_name_with_dirs plugin_value)
    string(REPLACE "/" ";" _tokens ${plugin_name_with_dirs})
    list(GET _tokens -1 plugin_name)
    string(REPLACE "." "_" plugin_target ${plugin_name})
    if("${my_filter}" STREQUAL "ALL")
      list(APPEND plugin_targets ${plugin_target})
    elseif("${my_filter}" STREQUAL "ON")
      if(${${plugin_name_with_dirs}_option_name})
        list(APPEND plugin_targets ${plugin_target})
      endif()
    else()
      if(NOT ${${plugin_name_with_dirs}_option_name})
        list(APPEND plugin_targets ${plugin_target})
      endif()
    endif()
  endforeach()

  set(${var_targets} ${plugin_targets} PARENT_SCOPE)

endfunction() 
