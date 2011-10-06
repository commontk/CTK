#
# ZMQ
#
SET(ZMQ_DEPENDS)
ctkMacroShouldAddExternalProject(ZMQ_LIBRARIES add_project)
IF(${add_project})

  # Sanity checks
  IF(DEFINED ZMQ_DIR AND NOT EXISTS ${ZMQ_DIR})
    MESSAGE(FATAL_ERROR "ZMQ_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(ZMQ_enabling_variable ZMQ_LIBRARIES)

  SET(proj ZMQ)
  SET(proj_DEPENDENCIES)

  SET(ZMQ_DEPENDS ${proj})

  IF(NOT DEFINED ZMQ_DIR)

    SET(revision_tag d2c2f96b49ed3835a47e)
    IF(${proj}_REVISION_TAG)
      SET(revision_tag ${${proj}_REVISION_TAG})
    ENDIF()

    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()

  #   MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY ${git_protocol}://github.com/PatrickCheng/zeromq2.git
      GIT_TAG ${revision_tag}
      UPDATE_COMMAND ""
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
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
    SET(ZMQ_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS ZMQ_DIR:PATH)

  SET(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_LIBRARY_DIRS)
  SET(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_INCLUDE_DIRS)
  SET(${ZMQ_enabling_variable}_FIND_PACKAGE_CMD ZMQ)
ENDIF()
