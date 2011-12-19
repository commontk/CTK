#
# ZMQ
#

ctkMacroShouldAddExternalproject(ZMQ_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED ZMQ_DIR AND NOT EXISTS ${ZMQ_DIR})
    message(FATAL_ERROR "ZMQ_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(ZMQ_enabling_variable ZMQ_LIBRARIES)

  set(proj ZMQ)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_LIBRARY_DIRS)
  set(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_INCLUDE_DIRS)
  set(${ZMQ_enabling_variable}_FIND_PACKAGE_CMD ZMQ)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED ZMQ_DIR)

      set(revision_tag d2c2f96b49ed3835a47e)
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()

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
        GIT_REPOSITORY ${git_protocol}://github.com/PatrickCheng/zeromq2.git
        GIT_TAG ${revision_tag}
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
          -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
          -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
          ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
          -DBUILD_TESTING:BOOL=OFF
          -DBUILD_SHARED_LIBS:BOOL=ON
          -DZMQ_BUILD_DEVICES:BOOL=ON
          -DZMQ_BUILD_PERFORMANCE_TESTS:BOOL=ON
         DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(ZMQ_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS ZMQ_DIR:PATH)

  endif()

endif()
