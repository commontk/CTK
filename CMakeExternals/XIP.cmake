#
# XIP
#

ctkMacroShouldAddExternalproject(XIP_LIBRARIES add_project)
if(${add_project})
  # Sanity checks
  if(DEFINED XIP_DIR AND NOT EXISTS ${XIP_DIR})
    message(FATAL_ERROR "XIP_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(XIP_enabling_variable XIP_LIBRARIES)

  set(proj XIP)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${XIP_enabling_variable}_INCLUDE_DIRS XIP_LIBRARY_DIRS)
  set(${XIP_enabling_variable}_INCLUDE_DIRS XIP_INCLUDE_DIRS)
  set(${XIP_enabling_variable}_FIND_PACKAGE_CMD XIP)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED XIP_DIR)
    
      set(location_args )
      if(${proj}_URL)
        set(location_args URL ${${proj}_URL})
      else()
        set(location_args SVN_REPOSITORY "https://collab01a.scr.siemens.com/svn/xip/releases/latest"
                          SVN_USERNAME "anonymous")
      endif()

      #   message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          -DHAS_VTK:BOOL=OFF
          -DHAS_ITK:BOOL=OFF
        )
      set(XIP_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS XIP_DIR:PATH)

  endif()

endif()
