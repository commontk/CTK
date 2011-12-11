#!
#! \brief Stores all target dependencies (potentially also from external projects)
#! in the variable specified by the first argument.
#!
#! \param var_deps (required) A variable name containing the output.
#! \param TARGETS (required) A list of targets (library targets or plug-in targets/symbolic names)
#!                for which the set of dependencies should be obtained.
#! \param ALL (option) Include external dependencies.
#! \ingroup CMakeUtilities
function(ctkFunctionGetTargetDependencies var_deps)

  ctkMacroParseArguments(MY "TARGETS" "ALL" ${ARGN})

  # Sanity checks
  if(NOT var_deps)
    message(FATAL_ERROR "Missing variable name as the first argument for storing the result")
  endif()
  
  if(NOT MY_TARGETS)
    message(FATAL_ERROR "Missing target names")
  endif()

  set(_targets )
  foreach(_target ${MY_TARGETS})
    # convenience conversion for plug-in targets
    string(REPLACE "." "_" _target ${_target})
    # assume the variable ${_target}_DEPENDENCIES was set during
    # a previous invocation of the ctkMacroValidateBuildOptions macro.
    list(APPEND _targets ${${_target}_DEPENDENCIES})
  endforeach()
  
  if (_targets)
    list(REMOVE_DUPLICATES _targets)
    if(NOT MY_ALL)
      # remove external targets not belonging to the current project
      ctkMacroGetAllProjectTargetLibraries("${_targets}" _targets)
    endif()
  endif()

  set(${var_deps} ${_targets} PARENT_SCOPE)

endfunction()
