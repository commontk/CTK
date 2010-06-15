#
# CTKData
#
SET(CTKData_DEPENDS)
IF(BUILD_TESTING)
  
  # Sanity checks
  IF(DEFINED CTKData_DIR AND NOT EXISTS ${CTKData_DIR})
    MESSAGE(FATAL_ERROR "CTKData_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  IF(NOT DEFINED CTKData_DIR)
    SET(proj CTKData)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(CTKData_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        GIT_REPOSITORY git://github.com/commontk/CTKData.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        )
	  SET(CTKData_DIR ${ep_source_dir}/${proj})
  ENDIF()
ENDIF()
