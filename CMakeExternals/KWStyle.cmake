#
# KWStyle
#

set(proj KWStyle)

set(${proj}_DEPENDENCIES "")

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(KWSTYLE_EXECUTABLE CACHE)
  find_program(KWSTYLE_EXECUTABLE kwstyle)
  if(NOT KWSTYLE_EXECUTABLE)
    message(FATAL_ERROR "Couldn't find 'kwstyle' on the system !")
  endif()
endif()

# Sanity checks
if(DEFINED KWSTYLE_EXECUTABLE AND NOT EXISTS ${KWSTYLE_EXECUTABLE})
  message(FATAL_ERROR "KWSTYLE_EXECUTABLE variable is defined but corresponds to non-existing executable")
endif()

if(NOT DEFINED KWSTYLE_EXECUTABLE)

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  else()
    set(location_args CVS_REPOSITORY ":pserver:anoncvs:@public.kitware.com:/cvsroot/KWStyle"
                      CVS_MODULE "KWStyle")
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS KWSTYLE_EXECUTABLE:FILEPATH
  LABELS "FIND_PACKAGE_VARS"
  )
