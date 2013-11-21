#
# QtTesting
#

ctk_include_once()

set(QtTesting_DEPENDS)

set(QtTesting_DEPENDENCIES "")

ctkMacroCheckExternalProjectDependency(QtTesting)
set(proj QtTesting)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED QtTesting_DIR AND NOT EXISTS ${QtTesting_DIR})
  message(FATAL_ERROR "QtTesting_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED QtTesting_DIR)

  set(revision_tag 7dbef1003157941e7315220f53cf4e17d78b5e28)
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
    set(location_args GIT_REPOSITORY "${git_protocol}://paraview.org/QtTesting.git"
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

  message(STATUS "Adding project:${proj}")
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    CMAKE_GENERATOR ${gen}
    UPDATE_COMMAND ""
    LIST_SEPARATOR ${sep}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(QtTesting_INSTALL_DIR ${ep_install_dir})
  set(QtTesting_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  # Since QtTesting is statically build, there is not need to add its corresponding
  # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS
  QtTesting_INSTALL_DIR:PATH
  QtTesting_DIR:PATH
  )
