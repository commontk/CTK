#
# CTKData
#
SET(CTKData_DEPENDS)
IF(BUILD_TESTING)
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
	  SET(CTKData_DIR ${ep_build_dir}/${proj})
	ENDIF()
ENDIF()
