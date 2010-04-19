PROJECT(CTKDICOMWidgets)

#
# See CTK/CMake/ctkMacroBuildQtLib.cmake for details
#

SET(KIT_export_directive "CTK_DICOM_WIDGETS_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  ctkDICOMListenerWidget.cpp
  ctkDICOMListenerWidget.h
  ctkDICOMLocationWidget.cpp
  ctkDICOMLocationWidget.h
  ctkDICOMQueryRetrieveWidget.cpp
  ctkDICOMQueryRetrieveWidget.h
  ctkDICOMServerNodeWidget.cpp
  ctkDICOMServerNodeWidget.h
  ctkDICOMQueryWidget.cpp
  ctkDICOMQueryWidget.h
  ctkDICOMCommandWidget.cpp
  ctkDICOMCommandWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  )

# UI files - includes new widgets
SET(KIT_UI_FORMS
  Resources/UI/ctkDICOMLocationWidget.ui
  Resources/UI/ctkDICOMListenerWidget.ui
  Resources/UI/ctkDICOMQueryRetrieveWidget.ui
  Resources/UI/ctkDICOMServerNodeWidget.ui
  Resources/UI/ctkDICOMQueryWidget.ui
  Resources/UI/ctkDICOMCommandWidget.ui
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

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
