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

  set(revision_tag 7406782e7d3babe95486b84ee97fc39c927a8c0f)
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
    set(location_args GIT_REPOSITORY "https://github.com/MEONMedical/Log4Qt.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(ep_cache_args)

  if(CTK_QT_VERSION VERSION_EQUAL "5")
    list(APPEND ep_cache_args
      -DQT_DIR:PATH=${Qt5_DIR}
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
      -DQT_DIR:PATH=${Qt6_DIR}
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
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      ${ep_cache_args}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(Log4Qt_INSTALL_DIR ${ep_install_dir})
  set(Log4Qt_DIR ${Log4Qt_INSTALL_DIR}/lib/cmake/Log4Qt/)

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  set(_lib_subdir bin)

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${CMAKE_BINARY_DIR}/${proj}-build/${_lib_subdir}/<CMAKE_CFG_INTDIR>)
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
  VARS Log4Qt_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
