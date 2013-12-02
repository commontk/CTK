#
# ZMQ
#

superbuild_include_once()

set(ZMQ_enabling_variable ZMQ_LIBRARIES)
set(${ZMQ_enabling_variable}_LIBRARY_DIRS ZMQ_LIBRARY_DIRS)
set(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_INCLUDE_DIRS)
set(${ZMQ_enabling_variable}_FIND_PACKAGE_CMD ZMQ)

set(ZMQ_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(ZMQ)
set(proj ZMQ)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED ZMQ_DIR AND NOT EXISTS ${ZMQ_DIR})
  message(FATAL_ERROR "ZMQ_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED ZMQ_DIR)

  set(revision_tag d2c2f96b49ed3835a47e)
  if(${proj}_REVISION_TAG)
    set(revision_tag ${${proj}_REVISION_TAG})
  endif()

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  elseif(${proj}_GIT_REPOSITORY)
    set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                      GIT_TAG ${revision_tag})
  else()
    set(location_args GIT_REPOSITORY "${git_protocol}://github.com/PatrickCheng/zeromq2.git"
                      GIT_TAG ${revision_tag})
  endif()

  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    LIST_SEPARATOR ${sep}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DZMQ_BUILD_DEVICES:BOOL=ON
      -DZMQ_BUILD_PERFORMANCE_TESTS:BOOL=ON
     DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(ZMQ_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS ZMQ_DIR:PATH)
