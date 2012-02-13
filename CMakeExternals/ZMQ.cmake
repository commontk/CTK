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
          ${ep_common_cache_args}
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
