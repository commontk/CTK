PROJECT(CTKCore)

# CMake modules
SET(CMAKE_MODULE_PATH ${CTKCore_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# CMake Macros
INCLUDE(CMake/ctkMacroBFDCheck.cmake)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${QTMOBILITY_INCLUDE_DIRS}
  )
  
# Source files
SET(KIT_SRCS
  ctkAbstractFactory.h
  ctkAbstractFactory.tpp
  ctkAbstractObjectFactory.h
  ctkAbstractObjectFactory.tpp
  ctkAbstractPluginFactory.h
  ctkAbstractPluginFactory.tpp
  ctkAbstractQObjectFactory.h
  ctkAbstractQObjectFactory.tpp
  ctkAbstractLibraryFactory.h
  ctkAbstractLibraryFactory.tpp
  ctkDependencyGraph.cpp
  ctkDependencyGraph.h
  ctkModelTester.cpp
  ctkModelTester.h
  ctkPimpl.h
  ctkSingleton.h
  ctkTransferFunction.cpp
  ctkTransferFunction.h
  ctkUtils.cpp
  ctkUtils.h

  # PluginFramework sources
  PluginFramework/ctkLDAPSearchFilter.cxx
  PluginFramework/ctkPlugin.cxx
  PluginFramework/ctkPluginArchive.cxx
  PluginFramework/ctkPluginConstants.cxx
  PluginFramework/ctkPluginContext.cxx
  PluginFramework/ctkPluginDatabase.cxx
  PluginFramework/ctkPluginDatabaseException.cxx
  PluginFramework/ctkPluginException.cxx
  PluginFramework/ctkPluginFramework.cxx
  PluginFramework/ctkPluginFrameworkContext.cxx
  PluginFramework/ctkPluginFrameworkContextPrivate.cxx
  PluginFramework/ctkPluginFrameworkPrivate.cxx
  PluginFramework/ctkPluginManager.cxx
  PluginFramework/ctkPluginManifest.cxx
  PluginFramework/ctkPluginPrivate.cxx
  PluginFramework/ctkPlugins.cxx
  PluginFramework/ctkServiceReference.cxx
  PluginFramework/ctkServiceRegistration.cxx
  #PluginFramework/ctkServiceRegistry.cxx
  PluginFramework/ctkPluginStorage.cxx
  PluginFramework/ctkVersion.cxx
  PluginFramework/ctkVersionRange.cxx

  # EventBus sources
  EventBus/ctkEvent.cxx
  EventBus/ctkEventConstants.cxx
  )

IF(CTK_HAVE_BFD)
  LIST(APPEND KIT_SRCS
    ctkBinaryFileDescriptor.cpp
    ctkBinaryFileDescriptor.h
    )
ENDIF()

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkModelTester.h
  ctkTransferFunction.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Create a MANIFEST.MF resource for the CTK Core,
# pretending that is is a plugin (the system plugin)
ctkMacroGeneratePluginManifest(KIT_SRCS
  SYMBOLIC_NAME "system.plugin"
  VERSION "0.9.9"
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
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

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
