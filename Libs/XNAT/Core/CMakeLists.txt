project(CTKXNATCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_XNAT_CORE_EXPORT")

set(KIT_SRCS
  ctkXnatAPI.cpp
  ctkXnatDataModel.cpp
  ctkXnatDefaultSchemaTypes.cpp
  ctkXnatException.cpp
  ctkXnatExperiment.cpp
  ctkXnatFile.cpp
  ctkXnatListModel.cpp
  ctkXnatLoginProfile.cpp
  ctkXnatObject.cpp
  ctkXnatObjectPrivate.cpp
  ctkXnatProject.cpp
  ctkXnatReconstruction.cpp
  ctkXnatReconstructionFolder.cpp
  ctkXnatResource.cpp
  ctkXnatScan.cpp
  ctkXnatScanFolder.cpp
  ctkXnatAssessor.cpp
  ctkXnatAssessorFolder.cpp
  ctkXnatSession.cpp
  ctkXnatSettings.cpp
  ctkXnatSubject.cpp
  ctkXnatTreeItem.cpp
  ctkXnatTreeItem_p.h
  ctkXnatTreeModel.cpp
)

# Files which should be processed by Qts moc
set(KIT_MOC_SRCS
  ctkXnatAPI_p.h
  ctkXnatSession.h
  ctkXnatListModel.h
  ctkXnatTreeModel.h
)


# Resources
set(KIT_resources
)

set(QT_USE_QTSCRIPT 1)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries)

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

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
