project(org_commontk_configadmin)

set(PLUGIN_export_directive "org_commontk_configadmin_EXPORT")

set(PLUGIN_SRCS
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
  ctkConfigurationEventAdapter_p.h
  ctkConfigurationEventAdapter.cpp
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
set(PLUGIN_MOC_SRCS
  ctkCMSerializedTaskQueue_p.h
  ctkConfigurationAdminActivator_p.h
  ctkConfigurationAdminFactory_p.h
  ctkConfigurationAdminImpl_p.h
  ctkConfigurationEventAdapter_p.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

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
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
