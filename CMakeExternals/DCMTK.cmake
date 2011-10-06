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
    SET(revision_tag 085525e643cab5ac82)
    IF(${proj}_REVISION_TAG)
      SET(revision_tag ${${proj}_REVISION_TAG})
    ENDIF()

#     MESSAGE(STATUS "Adding project:${proj}")
    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()
    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY "${git_protocol}://git.dcmtk.org/dcmtk.git"
      GIT_TAG ${revision_tag}
      CMAKE_GENERATOR ${gen}
      UPDATE_COMMAND ""
      BUILD_COMMAND ""
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
        -DDCMTK_WITH_ZLIB:BOOL=OFF # see github issue #25
        -DDCMTK_WITH_OPENSSL:BOOL=OFF # see github issue #25
        -DDCMTK_WITH_PNG:BOOL=OFF # see github issue #25
        -DDCMTK_WITH_TIFF:BOOL=OFF  # see github issue #25
        -DDCMTK_WITH_XML:BOOL=OFF  # see github issue #25
        -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
        ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        #-DBUILD_TESTING:BOOL=OFF # Not used
        -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
      )
    SET(DCMTK_DIR ${ep_install_dir})

# This was used during heavy development on DCMTK itself.
# Disabling it for now. (It also leads to to build errors
# with the XCode CMake generator on Mac).
#
#    ExternalProject_Add_Step(${proj} force_rebuild
#      COMMENT "Force ${proj} re-build"
#      DEPENDERS build    # Steps that depend on this step
#      ALWAYS 1
#      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${proj}-build
#      DEPENDS
#        ${proj_DEPENDENCIES}
#      )

    # Since DCMTK is statically build, there is not need to add its corresponding
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS DCMTK_DIR:PATH)

  SET(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
  SET(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)
ENDIF()
