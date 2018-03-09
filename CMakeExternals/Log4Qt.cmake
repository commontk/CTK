#
# Log4Qt
#

set(proj Log4Qt)

set(${proj}_DEPENDENCIES "")

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED Log4Qt_DIR AND NOT EXISTS ${Log4Qt_DIR})
  message(FATAL_ERROR "Log4Qt_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED Log4Qt_DIR)

  set(revision_tag 8d3558b0f636cbf8ff83)
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
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://github.com/commontk/Log4Qt.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(ep_cache_args)
  if(CTK_QT_VERSION VERSION_LESS "5")
    list(APPEND ep_cache_args
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      )
  else()
    list(APPEND ep_cache_args
      -DQt5_DIR:PATH=${Qt5_DIR}
      )
    # XXX Backward compatible way
    if(DEFINED CMAKE_PREFIX_PATH)
      list(APPEND ep_cache_args
        -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
        )
    endif()
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      ${ep_cache_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(Log4Qt_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS Log4Qt_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
