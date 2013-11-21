
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

ctkMacroCollectAllTargetLibraries("${CTK_LIBS}" "Libs" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_PLUGINS}" "Plugins" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_APPS}" "Applications" ALL_TARGET_LIBRARIES)
#message(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

#-----------------------------------------------------------------------------
# Initialize NON_CTK_DEPENDENCIES variable
#
# Using the variable ALL_TARGET_LIBRARIES initialized above with the help
# of the macro ctkMacroCollectAllTargetLibraries, let's get the list of all Non-CTK dependencies.
# NON_CTK_DEPENDENCIES is expected by the macro ctkMacroShouldAddExternalProject
ctkMacroGetAllNonProjectTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_CTK_DEPENDENCIES)
#message(NON_CTK_DEPENDENCIES:${NON_CTK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#

if(CTK_SUPERBUILD)
  include(ExternalProject)
  include(ctkMacroEmptyExternalProject)

  #set(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
  #set_property(DIRECTORY PROPERTY EP_BASE ${ep_base})

  set(ep_install_dir ${CMAKE_BINARY_DIR}/CMakeExternals/Install)
  set(ep_suffix      "-cmake")

  set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
  set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

  set(ep_common_cache_args
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
      -DBUILD_TESTING:BOOL=OFF
     )

  # Set CMake OSX variable to pass down the external projects
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
         -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
         -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
         -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
        )
    list(APPEND ep_common_cache_args ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS})
  endif()

  # Compute -G arg for configuring external projects with the same CMake generator:
  if(CMAKE_EXTRA_GENERATOR)
    set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
  else()
    set(gen "${CMAKE_GENERATOR}")
  endif()

  # Use this value where semi-colons are needed in ep_add args:
  set(sep "^^")

  # This variable will contain the list of CMake variable specific to each external project
  # that should passed to CTK.
  # The item of this list should have the following form: <EP_VAR>:<TYPE>
  # where '<EP_VAR>' is an external project variable and TYPE is either BOOL, PATH or FILEPATH.
  # Variable appended to this list will be automatically exported in CTKConfig.cmake, prefix 'CTK_'
  # will be prepended if it applied.
  set(CTK_SUPERBUILD_EP_VARS)
endif()

if(NOT DEFINED CTK_DEPENDENCIES)
  message(FATAL_ERROR "error: CTK_DEPENDENCIES variable is not defined !")
endif()

set(EXTERNAL_PROJECT_DIR ${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/CMakeExternals)
set(EXTERNAL_PROJECT_FILE_PREFIX "")
include(ctkMacroCheckExternalProjectDependency)

ctkMacroCheckExternalProjectDependency(CTK)

#message("Updated CTK_DEPENDENCIES:")
#foreach(dep ${CTK_DEPENDENCIES})
#  message("  ${dep}")
#endforeach()
