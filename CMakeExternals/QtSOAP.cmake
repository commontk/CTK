#
# QtSOAP
#

SET(QtSOAP_DEPENDS)
ctkMacroShouldAddExternalProject(QtSOAP_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED QtSOAP_DIR AND NOT EXISTS ${QtSOAP_DIR})
    MESSAGE(FATAL_ERROR "QtSOAP_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(QtSOAP_enabling_variable QtSOAP_LIBRARIES)

  SET(proj QtSOAP)
  SET(proj_DEPENDENCIES)

  SET(QtSOAP_DEPENDS ${proj})

  IF(NOT DEFINED QtSOAP_DIR)
  
    #     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY "${git_protocol}://github.com/commontk/QtSOAP.git"
      GIT_TAG "origin/master"
      CMAKE_GENERATOR ${gen}
      INSTALL_COMMAND ""
      CMAKE_ARGS
        ${ep_common_args}
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(QtSOAP_DIR "${ep_build_dir}/${proj}")
    
    # Since QtSOAP is statically build, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  SET(${QtSOAP_enabling_variable}_LIBRARY_DIRS QtSOAP_LIBRARY_DIRS)
  SET(${QtSOAP_enabling_variable}_INCLUDE_DIRS QtSOAP_INCLUDE_DIRS)
  SET(${QtSOAP_enabling_variable}_FIND_PACKAGE_CMD QtSOAP)
      
ENDIF()
