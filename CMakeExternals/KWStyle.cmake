#
# KWStyle
#

superbuild_include_once()

set(KWStyle_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(KWStyle)
set(proj KWStyle)

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
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    CMAKE_GENERATOR ${gen}
    LIST_SEPARATOR ${sep}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)

  # Since KWStyle is an executable, there is not need to add its corresponding
  # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS KWSTYLE_EXECUTABLE:PATH)
