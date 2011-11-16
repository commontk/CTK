
#
# Independently of the value of CTK_SUPERBUILD, each external project definition will
# provides either the include and library directories or a variable name
# used by the corresponding Find<package>.cmake files.
#
# Within top-level CMakeLists.txt file, the variable names will be expanded if not in
# superbuild mode. The include and library dirs are then used in
# ctkMacroBuildApp, ctkMacroBuildLib, and ctkMacroBuildPlugin
#

#-----------------------------------------------------------------------------
# Collect CTK library target dependencies
#

ctkMacroCollectAllTargetLibraries("${CTK_LIBS_SUBDIRS}" "Libs" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_PLUGINS_SUBDIRS}" "Plugins" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_APPLICATIONS_SUBDIRS}" "Applications" ALL_TARGET_LIBRARIES)
#MESSAGE(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

#-----------------------------------------------------------------------------
# Initialize NON_CTK_DEPENDENCIES variable
#
# Using the variable ALL_TARGET_LIBRARIES initialized above with the help
# of the macro ctkMacroCollectAllTargetLibraries, let's get the list of all Non-CTK dependencies.
# NON_CTK_DEPENDENCIES is expected by the macro ctkMacroShouldAddExternalProject
ctkMacroGetAllNonProjectTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_CTK_DEPENDENCIES)
#MESSAGE(STATUS NON_CTK_DEPENDENCIES:${NON_CTK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Include external projects
#
#-----------------------------------------------------------------------------
# Git protocole option
#
option(CTK_USE_GIT_PROTOCOL "If behind a firewall turn this OFF to use http instead." ON)
set(git_protocol "git")
if(NOT CTK_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
IF(CTK_SUPERBUILD)
  INCLUDE(ExternalProject)
  INCLUDE(ctkMacroEmptyExternalProject)

  #SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
  #SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${ep_base})

  SET(ep_install_dir ${CMAKE_BINARY_DIR}/CMakeExternals/Install)
  set(ep_suffix      "-cmake")

  set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
  set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

  # Compute -G arg for configuring external projects with the same CMake generator:
  IF(CMAKE_EXTRA_GENERATOR)
    SET(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
  ELSE()
    SET(gen "${CMAKE_GENERATOR}")
  ENDIF()

  # Use this value where semi-colons are needed in ep_add args:
  set(sep "^^")

  # This variable will contain the list of CMake variable specific to each external project
  # that should passed to CTK.
  # The item of this list should have the following form: <EP_VAR>:<TYPE>
  # where '<EP_VAR>' is an external project variable and TYPE is either BOOL, PATH or FILEPATH.
  # Variable appended to this list will be automatically exported in CTKConfig.cmake, prefix 'CTK_'
  # will be prepended if it applied.
  SET(CTK_SUPERBUILD_EP_VARS)
ENDIF()

IF(NOT DEFINED CTK_POSSIBLE_DEPENDENCIES)
  MESSAGE(FATAL_ERROR "error: CTK_POSSIBLE_DEPENDENCIES variable is not defined !")
ENDIF()
FOREACH(p ${CTK_POSSIBLE_DEPENDENCIES})
  INCLUDE(CMakeExternals/${p}.cmake)
ENDFOREACH()

#MESSAGE("CTK_POSSIBLE_DEPENDENCIES:")
#FOREACH(dep ${CTK_POSSIBLE_DEPENDENCIES})
#  MESSAGE("  ${dep}")
#ENDFOREACH()
