#!
#! \brief Stores all known plug-in dependencies (potentially also from external projects)
#! in the variable specified by the first argument.
#!
#! \param var_deps (required) A variable name containing the output.
#! \param PLUGINS (required) A list of plug-ins (target names or symbolic names) for which the
#!        set of dependencies should be obtained.
#! \param ALL (option) Include external dependencies.
#! \ingroup CMakeUtilities
function(ctkFunctionGetPluginDependencies var_deps)

  ctkMacroParseArguments(MY "PLUGINS" "ALL" ${ARGN})

  # Sanity checks
  if(NOT var_deps)
    message(FATAL_ERROR "Missing variable name as the first argument for storing the result")
  endif()
  
  if(NOT MY_PLUGINS)
    message(FATAL_ERROR "Missing plug-in names")
  endif()
  
  if(MY_ALL)
    ctkFunctionGetTargetDependencies(_targets TARGETS ${MY_PLUGINS} ALL)
  else()
    ctkFunctionGetTargetDependencies(_targets TARGETS ${MY_PLUGINS})
  endif()
  
  set(_plugins )
  foreach(_target ${_targets})
    if(_target MATCHES _)
      list(APPEND _plugins ${_target})
    endif()
  endforeach()

  set(${var_deps} ${_plugins} PARENT_SCOPE)

endfunction()
