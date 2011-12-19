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
      # Set CMake OSX variable to pass down the external project
      set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
      if(APPLE)
        list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
          -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
          -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
          -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET})
      endif()

      #   message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        SVN_REPOSITORY "https://collab01a.scr.siemens.com/svn/xip/releases/latest"
        SVN_USERNAME "anonymous"
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
          -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
          -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
          ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
          -DBUILD_TESTING:BOOL=OFF
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
