#
# qRestAPI
#

set(proj qRestAPI)

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
if(DEFINED ${proj}_DIR AND NOT EXISTS ${${proj}_DIR})
  message(FATAL_ERROR "${proj}_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED ${proj}_DIR)

  set(revision_tag "ddc0cfcc220d0ccd02b4afdd699d1e780dac3fa3")
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
    set(location_args GIT_REPOSITORY "https://github.com/commontk/qRestAPI.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(ep_cache_args)
  if(CTK_QT_VERSION VERSION_EQUAL "5")
    list(APPEND ep_cache_args
      -DQt5_DIR:PATH=${Qt5_DIR}
      )
    # XXX Backward compatible way
    if(DEFINED CMAKE_PREFIX_PATH)
      list(APPEND ep_cache_args
        -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
        )
    endif()
  elseif(CTK_QT_VERSION VERSION_EQUAL "6")
    list(APPEND ep_cache_args
      -DQt6_DIR:PATH=${Qt6_DIR}
      )
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
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
      -DqRestAPI_QT_VERSION:STRING=${CTK_QT_VERSION}
      ${ep_cache_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(qRestAPI_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build)

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${qRestAPI_DIR}/<CMAKE_CFG_INTDIR>)
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )

  #-----------------------------------------------------------------------------
  # Launcher setting specific to install tree

  # NA

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS qRestAPI_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
