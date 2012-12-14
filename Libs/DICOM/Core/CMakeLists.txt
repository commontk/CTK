project(CTKDICOMCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_DICOM_CORE_EXPORT")


# determine the DCMTK version we're using
if(EXISTS ${DCMTK_config_INCLUDE_DIR}/cfunix.h)
  set(_DCMTK_CONFIG_HEADER ${DCMTK_config_INCLUDE_DIR}/cfunix.h)
elseif(EXISTS ${DCMTK_config_INCLUDE_DIR}/osconfig.h)
  set(_DCMTK_CONFIG_HEADER ${DCMTK_config_INCLUDE_DIR}/osconfig.h)
else()
  message(SEND_ERROR "DCMTK version cannot be determined!")
endif()


file(READ ${_DCMTK_CONFIG_HEADER} _DCMTK_CONFIG)
# simple check, we do not expect anything else
if(_DCMTK_CONFIG MATCHES "PACKAGE_VERSION_NUMBER 360")
  set(DCMTK_VERSION_IS_360 1)
  message(STATUS "Detected DCMTK 3.6.0, will include backported SCU")
endif()

# Source files
set(KIT_SRCS
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
  ctkDICOMIndexer_p.h
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

if(DCMTK_VERSION_IS_360)
  list(APPEND KIT_SRCS ctkDcmSCU.cc)
endif(DCMTK_VERSION_IS_360)


# Abstract class should not be wrapped !
set_source_files_properties(
  ctkDICOMAbstractThumbnailGenerator.h
  WRAP_EXCLUDE
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.h
  ctkDICOMIndexer.h
  ctkDICOMIndexer_p.h
  ctkDICOMFilterProxyModel.h
  ctkDICOMModel.h
  ctkDICOMQuery.h
  ctkDICOMRetrieve.h
  ctkDICOMTester.h
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkDICOMCore.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

# create a dcm query/retrieve service config file that points to the build dir
set (DCMQRSCP_STORE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing)
configure_file( Resources/dcmqrscp.cfg.in dcmqrscp.cfg )
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

if(CTK_WRAP_PYTHONQT_FULL OR CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Plugins
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
