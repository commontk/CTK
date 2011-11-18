#
# OpenIGTLink
#

ctkMacroShouldAddExternalproject(OpenIGTLink_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED OpenIGTLink_DIR AND NOT EXISTS ${OpenIGTLink_DIR})
    message(FATAL_ERROR "OpenIGTLink_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(OpenIGTLink_enabling_variable OpenIGTLink_LIBRARIES)

  set(proj OpenIGTLink)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${OpenIGTLink_enabling_variable}_LIBRARY_DIRS OpenIGTLink_LIBRARY_DIRS)
  set(${OpenIGTLink_enabling_variable}_INCLUDE_DIRS OpenIGTLink_INCLUDE_DIRS)
  set(${OpenIGTLink_enabling_variable}_FIND_PACKAGE_CMD OpenIGTLink)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED OpenIGTLink_DIR)
      # Set CMake OSX variable to pass down the external project
      set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
      if(APPLE)
        list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
          -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
          -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
          -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
      endif()

    #   message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        SVN_REPOSITORY "http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink"
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
          -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
          -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
          ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(OpenIGTLink_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS OpenIGTLink_DIR:PATH)

  endif()

endif()
