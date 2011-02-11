PROJECT(CTKDICOMWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_DICOM_WIDGETS_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkDICOMDatasetView.cpp
  ctkDICOMDatasetView.h
  ctkDICOMDirectoryListWidget.cpp
  ctkDICOMDirectoryListWidget.h
  ctkDICOMListenerWidget.cpp
  ctkDICOMListenerWidget.h
  ctkDICOMQueryRetrieveWidget.cpp
  ctkDICOMQueryRetrieveWidget.h
  ctkDICOMServerNodeWidget.cpp
  ctkDICOMServerNodeWidget.h
  ctkDICOMQueryWidget.cpp
  ctkDICOMQueryWidget.h
  ctkDICOMQueryResultsTabWidget.cpp
  ctkDICOMQueryResultsTabWidget.h
  ctkDICOMAppWidget.cpp
  ctkDICOMAppWidget.h
  ctkDICOMThumbnailWidget.cpp
  ctkDICOMThumbnailWidget.h
  ctkDICOMThumbnailListWidget.cpp
  ctkDICOMThumbnailListWidget.h
  ctkDICOMImportWidget.cpp
  ctkDICOMImportWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkDICOMDatasetView.h
  ctkDICOMDirectoryListWidget.h
  ctkDICOMServerNodeWidget.h
  ctkDICOMAppWidget.h
  ctkDICOMThumbnailWidget.h
  ctkDICOMThumbnailListWidget.h
  ctkDICOMImportWidget.h
  ctkDICOMQueryRetrieveWidget.h
  )

# UI files - includes new widgets
SET(KIT_UI_FORMS
  Resources/UI/ctkDICOMDirectoryListWidget.ui
  Resources/UI/ctkDICOMListenerWidget.ui
  Resources/UI/ctkDICOMQueryRetrieveWidget.ui
  Resources/UI/ctkDICOMServerNodeWidget.ui
  Resources/UI/ctkDICOMQueryWidget.ui
  Resources/UI/ctkDICOMAppWidget.ui
  Resources/UI/ctkDICOMThumbnailWidget.ui
  Resources/UI/ctkDICOMThumbnailListWidget.ui
  Resources/UI/ctkDICOMImportWidget.ui
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
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
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
