#
# QtTesting
#

set(proj QtTesting)

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
if(DEFINED QtTesting_DIR AND NOT EXISTS ${QtTesting_DIR})
  message(FATAL_ERROR "QtTesting_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED QtTesting_DIR)

  if("${CMAKE_CXX_STANDARD}" STREQUAL "98")
    set(revision_tag c44b32fdea827be737e8c2f5608ffbc2e3bd08b2)
  else()
    set(revision_tag a86bee55104f553a1cb82b9cf0b109d9f1e95dbf) # ctk-2019-03-14-b5324a2
  endif()
  if(${proj}_REVISION_TAG)
    set(revision_tag ${${proj}_REVISION_TAG})
  endif()
  ExternalProject_Message(${proj} "${proj}[revision_tag:${revision_tag}]")

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  elseif(${proj}_GIT_REPOSITORY)
    set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                      GIT_TAG ${revision_tag})
  else()
    set(location_args GIT_REPOSITORY "https://github.com/commontk/QtTesting.git"
                      GIT_TAG ${revision_tag})
  endif()

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
  endif()

  set(ep_cache_args
    -DQtTesting_QT_VERSION:STRING=${CTK_QT_VERSION})
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
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      ${ep_cache_args}
      -DBUILD_SHARED_LIBS:BOOL=ON
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(QtTesting_INSTALL_DIR ${ep_install_dir})
  set(QtTesting_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${QtTesting_DIR}/<CMAKE_CFG_INTDIR>)
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
  VARS QtTesting_INSTALL_DIR:PATH
  LABELS "FIND_PACKAGE_VARS"
  )
mark_as_superbuild(
  VARS QtTesting_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
