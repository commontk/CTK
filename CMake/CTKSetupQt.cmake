#
#  Check if VTK was configured with QT, 
#  if so, use it, 
#  otherwise, complain. 
#


MACRO(ctk_setup_qt)

  IF(DEFINED CTK_QMAKE_EXECUTABLE)
    SET(QT_QMAKE_EXECUTABLE ${CTK_QMAKE_EXECUTABLE})
  ENDIF()

  SET(minimum_required_qt_version "4.6")

  FIND_PACKAGE(Qt4)
  IF(QT4_FOUND)

    IF(NOT "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" STREQUAL "${minimum_required_qt_version}")
      MESSAGE(FATAL_ERROR "error: CTK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    ENDIF()

    SET(QT_USE_QTNETWORK ON)
    SET(QT_USE_QTSQL ON)
    INCLUDE(${QT_USE_FILE})

    SET(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK libraries" FORCE)
    SET(CTK_QMAKE_EXECUTABLE ${QT_QMAKE_EXECUTABLE})
  ELSE(QT4_FOUND)
    MESSAGE(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
  ENDIF(QT4_FOUND)

ENDMACRO(ctk_setup_qt)
