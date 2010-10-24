#
# QtSOAP
#

SET(QtSOAP_DEPENDS)
ctkMacroShouldAddExternalProject(QTSOAP_LIBRARY add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED QtSOAP_DIR AND NOT EXISTS ${QtSOAP_DIR})
    MESSAGE(FATAL_ERROR "QtSOAP_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(QtSOAP_enabling_variable QTSOAP_LIBRARY)

  SET(proj QtSOAP)
  SET(proj_DEPENDENCIES)

  SET(QtSOAP_DEPENDS ${proj})

  IF(NOT DEFINED QtSOAP_DIR)
    IF(WIN32)
      SET(_qtsoap_url "${CTK_SOURCE_DIR}/Utilities/QtSOAP/qtsoap-2.7_1-opensource-win32.tar.gz")
      SET(_make_cmd nmake)
      IF(MINGW)
        SET(_make_cmd mingw32-make)
      ENDIF()
    ELSE()
      SET(_qtsoap_url "http://get.qt.nokia.com/qt/solutions/lgpl/qtsoap-2.7_1-opensource.tar.gz")
      SET(_make_cmd make)
    ENDIF()

    SET(_qtsoap_patch_script "${CTK_BINARY_DIR}/Utilities/QtSOAP/AcceptLicense.cmake")
    CONFIGURE_FILE("${CTK_SOURCE_DIR}/Utilities/QtSOAP/AcceptLicense.cmake.in" ${_qtsoap_patch_script} @ONLY)

    SET(_qtsoap_build_script "${CTK_BINARY_DIR}/Utilities/QtSOAP/BuildScript.cmake")
    CONFIGURE_FILE("${CTK_SOURCE_DIR}/Utilities/QtSOAP/BuildScript.cmake.in" ${_qtsoap_build_script} @ONLY)

    SET(_qtsoap_install_script "${CTK_BINARY_DIR}/Utilities/QtSOAP/InstallScript.cmake")
    CONFIGURE_FILE("${CTK_SOURCE_DIR}/Utilities/QtSOAP/InstallScript.cmake.in" ${_qtsoap_install_script} @ONLY)

    ExternalProject_Add(${proj}
      URL ${_qtsoap_url}
      PATCH_COMMAND ${CMAKE_COMMAND} -P ${_qtsoap_patch_script}
      CONFIGURE_COMMAND <SOURCE_DIR>/configure -library
      BUILD_IN_SOURCE 1
      BUILD_COMMAND ${CMAKE_COMMAND} -P ${_qtsoap_build_script}
      INSTALL_COMMAND ${CMAKE_COMMAND} -D INTERMEDIATE_DIRECTORY:STRING=$(IntDir) -P ${_qtsoap_install_script}
    )

    SET(QtSOAP_DIR "${CTK_BINARY_DIR}/Utilities/QtSOAP/")

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  SET(${QtSOAP_enabling_variable}_INCLUDE_DIRS QTSOAP_INCLUDE_DIR)
  SET(${QtSOAP_enabling_variable}_FIND_PACKAGE_CMD QtSOAP)
      
ENDIF()
