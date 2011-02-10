PROJECT(CTKDICOMCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_DICOM_CORE_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkDICOMImage.cpp
  ctkDICOMImage.h
  ctkDICOMIndexer.cpp
  ctkDICOMIndexer.h
  ctkDICOMDatabase.cpp
  ctkDICOMDatabase.h
  ctkDICOMModel.cpp
  ctkDICOMModel.h
  ctkDICOMQuery.cpp
  ctkDICOMQuery.h
  ctkDICOMRetrieve.cpp
  ctkDICOMRetrieve.h
  ctkDICOMDataset.cpp
  ctkDICOMDataset.h
  ctkDICOMPersonName.cpp
  ctkDICOMPersonName.h
)

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkDICOMDatabase.h
  ctkDICOMImage.h
  ctkDICOMModel.h
  ctkDICOMQuery.h
  ctkDICOMRetrieve.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
  Resources/dicom.qrc
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
