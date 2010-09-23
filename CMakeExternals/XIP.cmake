#
# XIP
#
SET (XIP_DEPENDS)
ctkMacroShouldAddExternalProject(XIP_LIBRARIES add_project)
IF(${add_project})
  SET(proj XIP)
#   MESSAGE(STATUS "Adding project:${proj}")
  SET(XIP_DEPENDS ${proj})
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://collab01a.scr.siemens.com/svn/xip/releases/latest"
    SVN_USERNAME "anonymous"
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${ep_common_args}
      -DHAS_VTK:BOOL=OFF
      -DHAS_ITK:BOOL=OFF
    )
ENDIF()
