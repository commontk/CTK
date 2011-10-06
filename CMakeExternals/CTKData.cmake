#
# CTKData
#
SET(CTKData_DEPENDS)
IF(BUILD_TESTING)

  # Sanity checks
  IF(DEFINED CTKData_DIR AND NOT EXISTS ${CTKData_DIR})
    MESSAGE(FATAL_ERROR "CTKData_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(proj CTKData)
  set(proj_DEPENDENCIES)

  SET(CTKData_DEPENDS ${proj})

  IF(NOT DEFINED CTKData_DIR)

    SET(revision_tag cc07f1ff391b7828459c)
    IF(${proj}_REVISION_TAG)
      SET(revision_tag ${${proj}_REVISION_TAG})
    ENDIF()

#    MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY ${git_protocol}://github.com/commontk/CTKData.git
      GIT_TAG ${revision_tag}
      UPDATE_COMMAND ""
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(CTKData_DIR ${CMAKE_BINARY_DIR}/${proj})
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS CTKData_DIR:PATH)
ENDIF()
