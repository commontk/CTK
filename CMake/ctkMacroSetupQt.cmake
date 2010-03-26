
#
#
#

MACRO(ctkMacroSetupQt)

  SET(minimum_required_qt_version "4.6")

  FIND_PACKAGE(Qt4)

  # This option won't show up in the main CMake configure panel
  MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)
  
  IF(QT4_FOUND)

    IF(NOT "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" STREQUAL "${minimum_required_qt_version}")
      MESSAGE(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    ENDIF()

    SET(QT_USE_QTNETWORK ON)
    SET(QT_USE_QTSQL ON)
    INCLUDE(${QT_USE_FILE})

  ELSE(QT4_FOUND)
    MESSAGE(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
  ENDIF(QT4_FOUND)

ENDMACRO()
