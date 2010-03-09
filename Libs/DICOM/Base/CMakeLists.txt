
PROJECT(qCTKDMCTK)

#
# See CMake/Slicer3QTBaseLibraryMacros.cmake for details
#

SET(KIT_export_directive "Q_CTKDMCTK_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  qCTKDCMTK.cxx
  qCTKDCMTK.h
  qCTKDCMTKListenerWidget.cxx
  qCTKDCMTKListenerWidget.h
  qCTKDCMTKLocationWidget.cxx
  qCTKDCMTKLocationWidget.h
  qCTKDCMTKQueryRetrieveWidget.cxx
  qCTKDCMTKQueryRetrieveWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  qCTKDCMTK.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/qCTKDCMTKLocationWidget.ui
  Resources/UI/qCTKDCMTKListenerWidget.ui
  Resources/UI/qCTKDCMTKQueryRetrieveWidget.ui
)

# Resources
SET(KIT_resources
)

# Additional Target libraries (QT libs already included)
SET(KIT_target_libraries
  ${QT_LIBRARIES}
  )

qctk_build_qtlib( 
  NAME ${PROJECT_NAME}
  EXPORT_TEMPLATE ${CTKDCMTK_SOURCE_DIR}/qCTKExport.h.in
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${qCTK_LIBRARY_MODE}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  #ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
