#
# KWStyle
#

if(CTK_USE_KWSTYLE)

  # Sanity checks
  if(DEFINED CTK_KWSTYLE_EXECUTABLE AND NOT EXISTS ${CTK_KWSTYLE_EXECUTABLE})
    message(FATAL_ERROR "CTK_KWSTYLE_EXECUTABLE variable is defined but corresponds to non-existing executable")
  endif()

  set(proj KWStyle-CVSHEAD)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  if(CTK_SUPERBUILD)

    if(NOT DEFINED CTK_KWSTYLE_EXECUTABLE)
    
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
        LIST_SEPARATOR ${sep}
        ${location_args}
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)

      # Since KWStyle is an executable, there is not need to add its corresponding
      # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS KWSTYLE_EXECUTABLE:PATH)

  endif()

endif()
