PROJECT(org_commontk_configadmin)

SET(PLUGIN_export_directive "org_commontk_configadmin_EXPORT")

SET(PLUGIN_SRCS
  ctkCMEventDispatcher.cpp
  ctkCMEventDispatcher_p.h
  ctkCMLogTracker.cpp
  ctkCMLogTracker_p.h
  ctkCMPluginManager.cpp
  ctkCMPluginManager_p.h
  ctkCMSerializedTaskQueue.cpp
  ctkCMSerializedTaskQueue_p.h
  ctkConfigurationAdminActivator.cpp
  ctkConfigurationAdminActivator_p.h
  ctkConfigurationAdminFactory.cpp
  ctkConfigurationAdminFactory_p.h
  ctkConfigurationAdminImpl.cpp
  ctkConfigurationAdminImpl_p.h
  ctkConfigurationImpl.cpp
  ctkConfigurationImpl_p.h
  ctkConfigurationStore.cpp
  ctkConfigurationStore_p.h
  ctkManagedServiceTracker.cpp
  ctkManagedServiceTracker_p.h
  ctkManagedServiceFactoryTracker.cpp
  ctkManagedServiceFactoryTracker_p.h
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkCMSerializedTaskQueue_p.h
  ctkConfigurationAdminActivator_p.h
  ctkConfigurationAdminFactory_p.h
  ctkConfigurationAdminImpl_p.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
)

#Compute the plugin dependencies
ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
