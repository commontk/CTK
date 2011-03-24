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
#     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY "${git_protocol}://itk.org/ITK.git"
      GIT_TAG "origin/master"
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
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
    SET(ITK_DIR ${ep_build_dir}/${proj})
    
    # Since the link directories associated with ITK is used, it makes sens to 
    # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
    LIST(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${ITK_DIR}/bin)
    
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  SET(${ITK_enabling_variable}_LIBRARY_DIRS ITK_LIBRARY_DIRS)
  SET(${ITK_enabling_variable}_INCLUDE_DIRS ITK_INCLUDE_DIRS)
  SET(${ITK_enabling_variable}_FIND_PACKAGE_CMD ITK)
ENDIF()

