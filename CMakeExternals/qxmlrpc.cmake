#
# qxmlrpc
#



set(proj qxmlrpc)

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
if(DEFINED qxmlrpc_DIR AND NOT EXISTS ${qxmlrpc_DIR})
  message(FATAL_ERROR "qxmlrpc_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED qxmlrpc_DIR)

  set(revision_tag 1d46d0e24d68049e726269dd3c6438671cd693ea)
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
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://github.com/commontk/qxmlrpc.git"
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
  set(qxmlrpc_DIR "${CMAKE_BINARY_DIR}/${proj}-build")

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS qxmlrpc_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
