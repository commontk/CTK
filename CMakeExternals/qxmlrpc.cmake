#
# qxmlrpc
#

superbuild_include_once()

set(qxmlrpc_enabling_variable qxmlrpc_LIBRARY)
set(${qxmlrpc_enabling_variable}_LIBRARY_DIRS qxmlrpc_LIBRARY_DIRS)
set(${qxmlrpc_enabling_variable}_INCLUDE_DIRS qxmlrpc_INCLUDE_DIRS)
set(${qxmlrpc_enabling_variable}_FIND_PACKAGE_CMD qxmlrpc)

set(qxmlrpc_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(qxmlrpc)
set(proj qxmlrpc)

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
    set(location_args GIT_REPOSITORY "${git_protocol}://github.com/commontk/qxmlrpc.git"
                      GIT_TAG ${revision_tag})
  endif()

  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    CMAKE_GENERATOR ${gen}
    INSTALL_COMMAND ""
    LIST_SEPARATOR ${sep}
    CMAKE_ARGS
      ${ep_common_cache_args}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(qxmlrpc_DIR "${CMAKE_BINARY_DIR}/${proj}-build")

  # Since qxmlrpc is statically build, there is not need to add its corresponding
  # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS qxmlrpc_DIR:PATH)
