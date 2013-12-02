#
# CTKData
#

superbuild_include_once()

set(CTKData_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(CTKData)
set(proj CTKData)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED CTKData_DIR AND NOT EXISTS ${CTKData_DIR})
  message(FATAL_ERROR "CTKData_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED CTKData_DIR)

  set(revision_tag cc07f1ff391b7828459c)
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
    set(location_args GIT_REPOSITORY "${git_protocol}://github.com/commontk/CTKData.git"
                      GIT_TAG ${revision_tag})
  endif()

  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    LIST_SEPARATOR ${sep}
    ${location_args}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(CTKData_DIR ${CMAKE_BINARY_DIR}/${proj})
else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS CTKData_DIR:PATH)
