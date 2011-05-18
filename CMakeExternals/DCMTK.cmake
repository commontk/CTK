#
# DCMTK
#

SET(DCMTK_DEPENDS)
ctkMacroShouldAddExternalProject(DCMTK_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED DCMTK_DIR AND NOT EXISTS ${DCMTK_DIR})
    MESSAGE(FATAL_ERROR "DCMTK_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(DCMTK_enabling_variable DCMTK_LIBRARIES)
  
  SET(proj DCMTK)
  SET(proj_DEPENDENCIES)
  
  SET(DCMTK_DEPENDS ${proj})
  
  IF(NOT DEFINED DCMTK_DIR)
#     MESSAGE(STATUS "Adding project:${proj}")

    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY "${git_protocol}://github.com/commontk/DCMTK.git"
      GIT_TAG "origin/patched"
      CMAKE_GENERATOR ${gen}
      BUILD_COMMAND ""
      CMAKE_ARGS
        ${ep_common_args}
        -DBUILD_TESTING:BOOL=OFF
        -DDCMTK_BUILD_APPS:BOOL=ON # Build also dmctk tools (movescu, storescp, ...)
      )
    SET(DCMTK_DIR ${ep_install_dir})

    ExternalProject_Add_Step(${proj} force_rebuild
      COMMENT "Force ${proj} re-build"
      DEPENDERS build    # Steps that depend on this step
      ALWAYS 1
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${proj}-build
      DEPENDS
        ${proj_DEPENDENCIES}
      )
      
    # Since DCMTK is statically build, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
  
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
  
  LIST(APPEND CTK_SUPERBUILD_EP_ARGS -DDCMTK_DIR:PATH=${DCMTK_DIR})

  SET(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
  SET(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)
ENDIF()
