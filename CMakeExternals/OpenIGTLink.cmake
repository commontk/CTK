#
# OpenIGTLink
#
SET (OpenIGTLink_DEPENDS)
ctkMacroShouldAddExternalProject(OpenIGTLink_LIBRARIES add_project)
IF(${add_project})
  IF(NOT DEFINED OpenIGTLink_DIR)
    SET(proj OpenIGTLink)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(OpenIGTLink_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        SVN_REPOSITORY "http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink"
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
          ${ep_common_args}
        )
    SET(OpenIGTLink_DIR ${ep_build_dir}/${proj})
  ENDIF()
ENDIF()