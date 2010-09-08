#
# QtSOAP
#

SET(QtSOAP_DEPENDS)
ctkMacroShouldAddExternalProject(QTSOAP_LIBRARIES add_project)
IF(${add_project})
  
  SET(proj QtSOAP)
#  MESSAGE(STATUS "Adding project:${proj}")
  SET(QtSOAP_DEPENDS ${proj})

  IF(WIN32)
    SET(_qtsoap_url "http://get.qt.nokia.com/qt/solutions/lgpl/qtsoap-2.7_1-opensource.zip")
    SET(_make_cmd nmake)
    IF(MINGW)
      SET(_make_cmd mingw32-make)
    ENDIF()
  ELSE()
    SET(_qtsoap_url "http://get.qt.nokia.com/qt/solutions/lgpl/qtsoap-2.7_1-opensource.tar.gz")
    SET(_make_cmd make)
  ENDIF()

  SET(_qtsoap_patch_script "${CTK_BINARY_DIR}/Utilities/QtSOAP/AcceptLicense.cmake")
  CONFIGURE_FILE("${CTK_SOURCE_DIR}/Utilities/QtSOAP/AcceptLicense.cmake.in" ${_qtsoap_patch_script})

  ExternalProject_Add(${proj}
    URL ${_qtsoap_url}
    PATCH_COMMAND ${CMAKE_COMMAND} -P ${_qtsoap_patch_script}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure -library
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${QT_QMAKE_EXECUTABLE}
    INSTALL_COMMAND ${_make_cmd} # misuse the install step for second build command
  )

  # Since the full path of PythonQt library is used, there is not need to add 
  # its corresponding library output directory to CTK_EXTERNAL_LIBRARY_DIRS
    
ENDIF()
