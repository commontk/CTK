
function(ctk_option_logical_expression_to_message varname logical_expr)
  set(enabling_msg)
  foreach(arg ${logical_expr})
    if(NOT "${${arg}}" STREQUAL "")
      set(value_as_int 0)
      if(${${arg}})
        set(value_as_int 1)
      endif()
      set(enabling_msg "${enabling_msg} ${arg}:${value_as_int}")
    else()
      set(enabling_msg "${enabling_msg} ${arg}")
    endif()
  endforeach()
  set(${varname} ${enabling_msg} PARENT_SCOPE)
endfunction()

macro(ctk_option option_prefix name doc default)
  option(${option_prefix}_${name} ${doc} ${default})
  mark_as_advanced(${option_prefix}_${name})
  list(APPEND ${option_prefix}S ${name})
  set(_logical_expr ${ARGN})
  if(_logical_expr AND NOT ${option_prefix}_${name})
    if(${ARGN})
      # Set the variable ON.
      set(${option_prefix}_${name} ON)
      # Generate user-friendly message
      set(enabling_msg)
      ctk_option_logical_expression_to_message(enabling_msg "${ARGN}")
      message(STATUS "Enabling [${option_prefix}_${name}] because of [${enabling_msg}] evaluates to True")
    endif()
  endif()
endmacro()

macro(ctk_lib_option name doc default)
  ctk_option(CTK_LIB ${name} ${doc} ${default} ${ARGN})
  if(CTK_BUILD_ALL_LIBRARIES)
    set(CTK_LIB_${name} 1)
  endif()
endmacro()

macro(ctk_plugin_option name doc default)
  ctk_option(CTK_PLUGIN ${name} ${doc} ${default} ${ARGN})
  if(CTK_BUILD_ALL_PLUGINS)
    set(CTK_PLUGIN_${name} 1)
  endif()
endmacro()

macro(ctk_app_option name doc default)
  ctk_option(CTK_APP ${name} ${doc} ${default} ${ARGN})
  if(CTK_BUILD_ALL_APPS)
    set(CTK_APP_${name} 1)
  endif()
endmacro()

macro(ctk_enable_option_raw name doc default)
  option(${name} "${doc}" ${default})
  if(DEFINED ${name}_internal)
    if(${${name}} AND ${${name}_internal})
      if(NOT (${ARGN}))
        get_property(_doc_string CACHE ${name} PROPERTY HELPSTRING)
        set(${name} OFF CACHE BOOL ${_doc_string} FORCE)
        message("Full support for [${name}] disabled")
      endif()
    endif()
  endif()
  set(${name}_internal ${${name}} CACHE INTERNAL "" FORCE)
endmacro()

macro(ctk_enable_option name doc default)
  ctk_enable_option_raw(CTK_ENABLE_${name} ${doc} ${default} ${ARGN})
endmacro()
