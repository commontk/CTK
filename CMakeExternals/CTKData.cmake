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
  #   MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY ${git_protocol}://github.com/commontk/CTKData.git
      GIT_TAG "origin/master"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      DEPENDS
        ${proj_DEPENDENCIES}
      )
	  SET(CTKData_DIR ${ep_source_dir}/${proj})
	ELSE()
	  ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
ENDIF()
