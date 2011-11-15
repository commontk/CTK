PROJECT(CTKDICOMCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_DICOM_CORE_EXPORT")


# Source files
SET(KIT_SRCS
  ctkDICOMAbstractThumbnailGenerator.cpp
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.cpp
  ctkDICOMDatabase.h
  ctkDICOMDataset.cpp
  ctkDICOMDataset.h
  ctkDICOMFilterProxyModel.cpp
  ctkDICOMFilterProxyModel.h
  ctkDICOMIndexer.cpp
  ctkDICOMIndexer.h
  ctkDICOMModel.cpp
  ctkDICOMModel.h
  ctkDICOMPersonName.cpp
  ctkDICOMPersonName.h
  ctkDICOMQuery.cpp
  ctkDICOMQuery.h
  ctkDICOMRetrieve.cpp
  ctkDICOMRetrieve.h
  ctkDICOMTester.cpp
  ctkDICOMTester.h
)

# Abstract class should not be wrapped !
SET_SOURCE_FILES_PROPERTIES(
  ctkDICOMAbstractThumbnailGenerator.h
  WRAP_EXCLUDE
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.h
  ctkDICOMIndexer.h
  ctkDICOMFilterProxyModel.h
  ctkDICOMModel.h
  ctkDICOMQuery.h
  ctkDICOMRetrieve.h
  ctkDICOMTester.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
  Resources/ctkDICOMCore.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

# create a dcm query/retrieve service config file that points to the build dir
set (DCMQRSCP_STORE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing)
CONFIGURE_FILE( Resources/dcmqrscp.cfg.in dcmqrscp.cfg )
set (DCMQRSCP_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/dcmqrscp.cfg)

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

IF(CTK_WRAP_PYTHONQT_FULL OR CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
ENDIF()

# Plugins
#IF(BUILD_QTDESIGNER_PLUGINS)
#  ADD_SUBDIRECTORY(Plugins)
#ENDIF()

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
