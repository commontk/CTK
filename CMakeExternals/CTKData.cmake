#
# CTKData
#

if(BUILD_TESTING)
  # Sanity checks
  if(DEFINED CTKData_DIR AND NOT EXISTS ${CTKData_DIR})
    message(FATAL_ERROR "CTKData_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(proj CTKData)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  if(CTK_SUPERBUILD)

    if(NOT DEFINED CTKData_DIR)

      set(revision_tag cc07f1ff391b7828459c)
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()

  #    message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY ${git_protocol}://github.com/commontk/CTKData.git
        GIT_TAG ${revision_tag}
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(CTKData_DIR ${CMAKE_BINARY_DIR}/${proj})
    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS CTKData_DIR:PATH)

  endif()

endif()
