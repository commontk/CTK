#
# XIP
#
SET (XIP_DEPENDS)
ctkMacroShouldAddExternalProject(XIP_LIBRARIES add_project)
IF(${add_project})
  # Sanity checks
  IF(DEFINED XIP_DIR AND NOT EXISTS ${XIP_DIR})
    MESSAGE(FATAL_ERROR "XIP_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  SET(XIP_enabling_variable XIP_LIBRARIES)
  
  SET(proj XIP)
  SET(proj_DEPENDENCIES)
  
  SET(XIP_DEPENDS ${proj})
  
  IF(NOT DEFINED XIP_DIR)
    #   MESSAGE(STATUS "Adding project:${proj}")
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
    SET(XIP_DIR ${ep_build_dir}/${proj})
    
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
  
  SET(${XIP_enabling_variable}_INCLUDE_DIRS XIP_LIBRARY_DIRS)
  SET(${XIP_enabling_variable}_INCLUDE_DIRS XIP_INCLUDE_DIRS)
  SET(${XIP_enabling_variable}_FIND_PACKAGE_CMD XIP)
  
ENDIF()
