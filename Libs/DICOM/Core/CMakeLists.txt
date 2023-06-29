project(CTKDICOMCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_DICOM_CORE_EXPORT")

# Source files
set(KIT_SRCS
  ctkDICOMAbstractThumbnailGenerator.cpp
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.cpp
  ctkDICOMDatabase.h
  ctkDICOMDatabase_p.h
  ctkDICOMItem.h
  ctkDICOMDisplayedFieldGenerator.cpp
  ctkDICOMDisplayedFieldGenerator.h
  ctkDICOMFilterProxyModel.cpp
  ctkDICOMFilterProxyModel.h
  ctkDICOMIndexer.cpp
  ctkDICOMIndexer.h
  ctkDICOMIndexer_p.h
  ctkDICOMItem.cpp
  ctkDICOMItem.h
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
  ctkDICOMUtil.cpp
  ctkDICOMUtil.h
  ctkDICOMDisplayedFieldGeneratorRuleFactory.h
  ctkDICOMDisplayedFieldGeneratorRuleFactory.cpp
  ctkDICOMDisplayedFieldGeneratorAbstractRule.h
  ctkDICOMDisplayedFieldGeneratorDefaultRule.h
  ctkDICOMDisplayedFieldGeneratorDefaultRule.cpp
  ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule.h
  ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule.cpp
  ctkDICOMDisplayedFieldGeneratorLastStudyDateRule.h
  ctkDICOMDisplayedFieldGeneratorLastStudyDateRule.cpp
  ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule.h
  ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule.cpp
  ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule.h
  ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule.cpp
  ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule.h
  ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule.cpp
)

# Abstract class should not be wrapped !
set_source_files_properties(
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDisplayedFieldGeneratorAbstractRule.h
  WRAP_EXCLUDE
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.h
  ctkDICOMDisplayedFieldGenerator.h
  ctkDICOMDisplayedFieldGenerator_p.h
  ctkDICOMDisplayedFieldGeneratorRuleFactory.h
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

if(TARGET DCMTK::dcmnet AND TARGET DCMTK::dcmjpeg)
  if(DCMTK_LIBRARIES)
    list(REMOVE_ITEM KIT_target_libraries ${DCMTK_LIBRARIES})
  endif()
  list(APPEND KIT_target_libraries DCMTK::dcmnet DCMTK::dcmjpeg)
endif()

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::Sql)
endif()

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

if(DEFINED DCMTK_HAVE_CONFIG_H_OPTIONAL AND NOT DCMTK_HAVE_CONFIG_H_OPTIONAL)
  # Workaround Debian packaging issue - See FindDCMTK.cmake for more details
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})
  if("${CMAKE_VERSION}" VERSION_GREATER 2.8.10)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})
  endif()
endif()


if(CTK_WRAP_PYTHONQT_LIGHT)
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
