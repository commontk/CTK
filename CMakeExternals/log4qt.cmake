#
# log4qt
#
SET(log4qt_DEPENDS)
ctkMacroShouldAddExternalProject(log4qt_LIBRARIES add_project)
IF(${add_project})

  # Sanity checks
  IF(DEFINED log4qt_DIR AND NOT EXISTS ${log4qt_DIR})
    MESSAGE(FATAL_ERROR "log4qt_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  IF(NOT DEFINED log4qt_DIR)
    SET(proj log4qt)
#     MESSAGE(STATUS "Adding project:${proj}")
    SET(log4qt_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        DOWNLOAD_COMMAND ""
        CMAKE_GENERATOR ${gen}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/${proj}
        CMAKE_ARGS
          ${ep_common_args}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        )
    SET(log4qt_DIR ${ep_install_dir})
      
  ENDIF()
ENDIF()
