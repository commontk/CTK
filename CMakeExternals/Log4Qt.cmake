#
# Log4Qt
#
SET(Log4Qt_DEPENDS)
ctkMacroShouldAddExternalProject(Log4Qt_LIBRARIES add_project)
IF(${add_project})

  # Sanity checks
  IF(DEFINED Log4Qt_DIR AND NOT EXISTS ${Log4Qt_DIR})
    MESSAGE(FATAL_ERROR "Log4Qt_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(Log4Qt_enabling_variable Log4Qt_LIBRARIES)
  
  SET(proj Log4Qt)
  SET(proj_DEPENDENCIES)
  
  SET(Log4Qt_DEPENDS ${proj})
  
  IF(NOT DEFINED Log4Qt_DIR)
#     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY "${git_protocol}://github.com/commontk/Log4Qt.git"
      GIT_TAG "origin/patched"
      CMAKE_GENERATOR ${gen}
      BUILD_COMMAND ""
      CMAKE_ARGS
        ${ep_common_args}
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(Log4Qt_DIR ${ep_install_dir})
    
    # Since Log4Qt is statically build, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
    
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  SET(${Log4Qt_enabling_variable}_INCLUDE_DIRS Log4Qt_INCLUDE_DIR)
  SET(${Log4Qt_enabling_variable}_FIND_PACKAGE_CMD Log4Qt)

ENDIF()
