project(CTKDICOMWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_DICOM_WIDGETS_EXPORT")


# Source files
set(KIT_SRCS
  ctkDICOMAppWidget.cpp
  ctkDICOMAppWidget.h
  ctkDICOMBrowser.cpp
  ctkDICOMBrowser.h
  ctkDICOMObjectModel.cpp
  ctkDICOMObjectModel.h
  ctkDICOMDirectoryListWidget.cpp
  ctkDICOMDirectoryListWidget.h
  ctkDICOMImage.cpp
  ctkDICOMImage.h
  ctkDICOMImportWidget.cpp
  ctkDICOMImportWidget.h
  ctkDICOMItemView.cpp
  ctkDICOMItemView.h
  ctkDICOMListenerWidget.cpp
  ctkDICOMListenerWidget.h
  ctkDICOMQueryResultsTabWidget.cpp
  ctkDICOMQueryResultsTabWidget.h
  ctkDICOMQueryRetrieveWidget.cpp
  ctkDICOMQueryRetrieveWidget.h
  ctkDICOMQueryWidget.cpp
  ctkDICOMQueryWidget.h
  ctkDICOMObjectListWidget.cpp
  ctkDICOMObjectListWidget.h
  ctkDICOMPatientItemWidget.cpp
  ctkDICOMPatientItemWidget.h
  ctkDICOMSeriesItemWidget.cpp
  ctkDICOMSeriesItemWidget.h
  ctkDICOMServerNodeWidget.cpp
  ctkDICOMServerNodeWidget.h
  ctkDICOMServerNodeWidget2.cpp
  ctkDICOMServerNodeWidget2.h
  ctkDICOMStudyItemWidget.cpp
  ctkDICOMStudyItemWidget.h
  ctkDICOMTableManager.h
  ctkDICOMTableManager.cpp
  ctkDICOMTableView.cpp
  ctkDICOMTableView.h
  ctkDICOMThumbnailGenerator.cpp
  ctkDICOMThumbnailGenerator.h
  ctkDICOMThumbnailListWidget.cpp
  ctkDICOMThumbnailListWidget.h
  ctkDICOMVisualBrowserWidget.cpp
  ctkDICOMVisualBrowserWidget.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkDICOMAppWidget.h
  ctkDICOMBrowser.h
  ctkDICOMItemView.h
  ctkDICOMDirectoryListWidget.h
  ctkDICOMImage.h
  ctkDICOMImportWidget.h
  ctkDICOMObjectListWidget.h
  ctkDICOMObjectModel.h
  ctkDICOMQueryRetrieveWidget.h
  ctkDICOMQueryWidget.h
  ctkDICOMPatientItemWidget.h
  ctkDICOMSeriesItemWidget.h
  ctkDICOMServerNodeWidget.h
  ctkDICOMServerNodeWidget2.h
  ctkDICOMStudyItemWidget.h
  ctkDICOMTableManager.h
  ctkDICOMTableView.h
  ctkDICOMThumbnailGenerator.h
  ctkDICOMThumbnailListWidget.h
  ctkDICOMVisualBrowserWidget.h
  )

# UI files - includes new widgets
set(KIT_UI_FORMS
  Resources/UI/ctkDICOMAppWidget.ui
  Resources/UI/ctkDICOMBrowser.ui
  Resources/UI/ctkDICOMDirectoryListWidget.ui
  Resources/UI/ctkDICOMImportWidget.ui
  Resources/UI/ctkDICOMListenerWidget.ui
  Resources/UI/ctkDICOMQueryRetrieveWidget.ui
  Resources/UI/ctkDICOMQueryWidget.ui
  Resources/UI/ctkDICOMObjectListWidget.ui
  Resources/UI/ctkDICOMPatientItemWidget.ui
  Resources/UI/ctkDICOMSeriesItemWidget.ui
  Resources/UI/ctkDICOMServerNodeWidget.ui
  Resources/UI/ctkDICOMServerNodeWidget2.ui
  Resources/UI/ctkDICOMStudyItemWidget.ui
  Resources/UI/ctkDICOMTableManager.ui
  Resources/UI/ctkDICOMTableView.ui
  Resources/UI/ctkDICOMVisualBrowserWidget.ui
)

# Resources
set(KIT_resources
  Resources/UI/ctkDICOMWidget.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

list(APPEND KIT_target_libraries Qt5::Svg)

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

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Plugins
if(CTK_BUILD_QTDESIGNER_PLUGINS)
  add_subdirectory(Plugins)
endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
