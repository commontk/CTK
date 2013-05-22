function(ctkFunctionAddPluginRepo)

  ctkMacroParseArguments("" "NAME;GIT_URL;GIT_TAG;GIT_PROTOCOL" "" ${ARGN})

  foreach(_required_arg NAME)
    if(NOT _${_required_arg})
      message(FATAL_ERROR "${_required_arg} is empty")
    endif()
  endforeach()

  if(NOT _GIT_URL AND NOT ${_NAME}_DIR)
    message(FATAL_ERROR "Either ${_NAME}_DIR or GIT_URL must be set")
  endif()

  if(NOT ${_NAME}_DIR)
    ctkFunctionCheckoutRepo(
      NAME ${_NAME}
      GIT_URL ${_GIT_URL}
      GIT_TAG ${_GIT_TAG}
      GIT_PROTOCOL ${_GIT_PROTOCOL}
      )
  endif()

  set(_gitmodules_files ${${_NAME}_DIR}/.gitmodules)
  if(NOT EXISTS ${_gitmodules_files})
    message(FATAL_ERROR "The repository at ${${_NAME}_DIR} does not contain a .gitmodules file")
  endif()

  # Parse the .gitmodules file and add the submodules as contributed plugins
  file(STRINGS "${_gitmodules_files}" _plugin_paths REGEX "path =.*")
  foreach(_plugin_path ${_plugin_paths})
    string(REPLACE " = " ";" _plugin_path_list ${_plugin_path})
    list(GET _plugin_path_list 1 _plugin_name)
    ctk_plugin_option(${_plugin_name} "Build the ${_plugin_name} plugin." OFF)

    # Push the value which might have been changed in ctk_plugin_option to the parent scope
    set(CTK_PLUGIN_${_plugin_name} ${CTK_PLUGIN_${_plugin_name}} PARENT_SCOPE)

    set(${_plugin_name}_SOURCE_DIR ${${_NAME}_DIR}/${_plugin_name})
    set(${_plugin_name}_SOURCE_DIR ${${_plugin_name}_SOURCE_DIR} PARENT_SCOPE)

    if(CTK_PLUGIN_${_plugin_name} AND NOT EXISTS ${${_plugin_name}_SOURCE_DIR})
      execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update ${_plugin_name}
        WORKING_DIRECTORY ${${_NAME}_DIR}
        RESULT_VARIABLE return_code
        ERROR_VARIABLE error_msg
      )
      if(return_code)
        message(FATAL_ERROR "Could not invoke git submodule update for ${${_plugin_name}_SOURCE_DIR}")
      endif()
    endif()
  endforeach()

  set(CTK_PLUGINS ${CTK_PLUGINS} PARENT_SCOPE)
  set(${_NAME}_DIR ${${_NAME}_DIR} PARENT_SCOPE)

endfunction()
