PROJECT(CTKPluginFramework)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_PLUGINFW_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkLDAPSearchFilter.cpp
  ctkPlugin.cpp
  ctkPluginArchive.cpp
  ctkPluginConstants.cpp
  ctkPluginContext.cpp
  ctkPluginDatabase.cpp
  ctkPluginDatabaseException.cpp
  ctkPluginEvent.cpp
  ctkPluginException.cpp
  ctkPluginFramework.cpp
  ctkPluginFrameworkFactory.cpp
  ctkPluginFrameworkContext.cpp
  ctkPluginFrameworkEvent.cpp
  ctkPluginFrameworkListeners.cpp
  ctkPluginFrameworkPrivate.cpp
  ctkPluginFrameworkUtil.cpp
  ctkPluginManifest.cpp
  ctkPluginPrivate.cpp
  ctkPlugins.cpp
  ctkRequirePlugin.cpp
  ctkServiceEvent.cpp
  ctkServiceException.cpp
  ctkServiceFactory.h
  ctkServiceReference.cpp
  ctkServiceReferencePrivate.cpp
  ctkServiceRegistration.cpp
  ctkServiceRegistrationPrivate.cpp
  ctkServices.cpp
  ctkServiceSlotEntry.cpp
  ctkPluginStorage.cpp
  ctkVersion.cpp
  ctkVersionRange.cpp
  ctkLDAPExpr.cpp

  # EventBus sources
  EventBus/ctkEvent.cpp
  EventBus/ctkEventBus.h
  EventBus/ctkEventConstants.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkPluginFrameworkListeners_p.h
  ctkServiceFactory.h
  ctkTrackedPluginListener_p.h
  ctkTrackedServiceListener_p.h
)

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Create a MANIFEST.MF resource for the PluginFramework library,
# pretending that is is a plugin (the system plugin)
ctkFunctionGeneratePluginManifest(KIT_SRCS
  SYMBOLIC_NAME "system.plugin"
  VERSION "0.9.9"
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

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
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
