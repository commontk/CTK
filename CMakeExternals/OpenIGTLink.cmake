#
# OpenIGTLink
#
SET (OpenIGTLink_DEPENDS)
ctkMacroShouldAddExternalProject(OpenIGTLink_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED OpenIGTLink_DIR AND NOT EXISTS ${OpenIGTLink_DIR})
    MESSAGE(FATAL_ERROR "OpenIGTLink_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  SET(proj OpenIGTLink)
  SET(proj_DEPENDENCIES)
  
  SET(OpenIGTLink_DEPENDS ${proj})
  
  IF(NOT DEFINED OpenIGTLink_DIR)
    
  #   MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      SVN_REPOSITORY "http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink"
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(OpenIGTLink_DIR ${ep_build_dir}/${proj})
  
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
ENDIF()
