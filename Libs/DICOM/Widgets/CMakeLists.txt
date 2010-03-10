PROJECT(CTKDICOMWidgets)

#
# See CTK/CMake/CTKBuildQtLibMacro.cmake for details
#

SET(KIT_export_directive "Q_CTK_DICOM_WIDGETS_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  qCTKDCMTKListenerWidget.cxx
  qCTKDCMTKListenerWidget.h
  qCTKDCMTKLocationWidget.cxx
  qCTKDCMTKLocationWidget.h
  qCTKDCMTKQueryRetrieveWidget.cxx
  qCTKDCMTKQueryRetrieveWidget.h
  qCTKDCMTKServerNodeWidget.cxx
  qCTKDCMTKServerNodeWidget.h
  qCTKDCMTKQueryWidget.cxx
  qCTKDCMTKQueryWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  )

# UI files - includes new widgets
SET(KIT_UI_FORMS
  Resources/UI/qCTKDCMTKLocationWidget.ui
  Resources/UI/qCTKDCMTKListenerWidget.ui
  Resources/UI/qCTKDCMTKQueryRetrieveWidget.ui
  Resources/UI/qCTKDCMTKServerNodeWidget.ui
  Resources/UI/qCTKDCMTKQueryWidget.ui
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
ctkMacroGetTargetLibraries(KIT_target_libraries)
  
# See CMake/ctkMacroBuildQtLib.cmake
ctkMacroBuildQtLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  #ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
