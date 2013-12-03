#
# ITK
#

superbuild_include_once()

set(proj ITK)

set(${proj}_DEPENDENCIES "")

superbuild_include_dependencies(PROJECT_VAR proj)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(ITK_DIR CACHE)
  find_package(ITK REQUIRED NO_MODULE)
endif()

# Sanity checks
if(DEFINED ITK_DIR AND NOT EXISTS ${ITK_DIR})
  message(FATAL_ERROR "ITK_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED ITK_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

  set(revision_tag "v3.20.1")
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
    set(location_args GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(ep_project_include_arg)
  if(CTEST_USE_LAUNCHERS)
    set(ep_project_include_arg
      "-DCMAKE_PROJECT_ITK_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
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
      ${ep_project_include_arg}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DITK_USE_REVIEW:BOOL=ON
      -DITK_USE_REVIEW_STATISTICS:BOOL=ON
      -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON
      -DITK_USE_PORTABLE_ROUND:BOOL=ON
      -DITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY:BOOL=ON
      -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON
      -DITK_LEGACY_REMOVE:BOOL=ON
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  # Since the link directories associated with ITK is used, it makes sens to
  # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
  list(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${ITK_DIR}/bin)

else()
  superbuild_add_empty_external_project(${proj} "${${proj}_DEPENDENCIES}")
endif()

mark_as_superbuild(
  VARS ITK_DIR:PATH
  LABELS "FIND_PACKAGE"
  )
