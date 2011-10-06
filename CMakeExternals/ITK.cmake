#
# ITK
#
SET (ITK_DEPENDS)
ctkMacroShouldAddExternalProject(ITK_LIBRARIES add_project)
IF(${add_project})
  # Sanity checks
  IF(DEFINED ITK_DIR AND NOT EXISTS ${ITK_DIR})
    MESSAGE(FATAL_ERROR "ITK_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(ITK_enabling_variable ITK_LIBRARIES)

  SET(proj ITK)
  SET(proj_DEPENDENCIES)

  SET(ITK_DEPENDS ${proj})

  IF(NOT DEFINED ITK_DIR)

    SET(revision_tag "v3.20.0")
    IF(${proj}_REVISION_TAG)
      SET(revision_tag ${${proj}_REVISION_TAG})
    ENDIF()

    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()

#     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
      GIT_TAG ${revision_tag}
      UPDATE_COMMAND ""
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
        ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        -DBUILD_TESTING:BOOL=OFF
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
        ${proj_DEPENDENCIES}
      )
    SET(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    # Since the link directories associated with ITK is used, it makes sens to
    # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
    LIST(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${ITK_DIR}/bin)

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS ITK_DIR:PATH)

  SET(${ITK_enabling_variable}_LIBRARY_DIRS ITK_LIBRARY_DIRS)
  SET(${ITK_enabling_variable}_INCLUDE_DIRS ITK_INCLUDE_DIRS)
  SET(${ITK_enabling_variable}_FIND_PACKAGE_CMD ITK)
ENDIF()

