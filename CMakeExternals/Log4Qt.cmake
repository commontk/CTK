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
  
  IF(NOT DEFINED Log4Qt_DIR)
    SET(proj Log4Qt)
#     MESSAGE(STATUS "Adding project:${proj}")
    SET(Log4Qt_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        GIT_REPOSITORY "git://github.com/commontk/Log4Qt.git"
        GIT_TAG "patched"
        CMAKE_GENERATOR ${gen}
        BUILD_COMMAND ""
        CMAKE_ARGS
          ${ep_common_args}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        )
    SET(Log4Qt_DIR ${ep_install_dir})
    
    # Since Log4Qt is statically build, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
      
  ENDIF()
ENDIF()
