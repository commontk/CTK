project(org_commontk_eventbus)

set(PLUGIN_export_directive "org_commontk_eventbus_EXPORT")

set(PLUGIN_SRCS
  ctkBusEvent.cpp
  ctkBusEvent.h
  ctkEventAdminBus.h
  ctkEventBus_global.h
  ctkEventBusImpl.cpp
  ctkEventBusImpl_p.h
  ctkEventBusManager.cpp
  ctkEventBusManager.h
  ctkEventBusPlugin.cpp
  ctkEventBusPlugin_p.h
  ctkEventDefinitions.h
  ctkEventDispatcher.cpp
  ctkEventDispatcher.h
  ctkEventDispatcherLocal.cpp
  ctkEventDispatcherLocal.h
  ctkEventDispatcherRemote.cpp
  ctkEventDispatcherRemote.h
  ctkEventHandlerWrapper_p.h
  ctkNetworkConnector.cpp
  ctkNetworkConnector.h
  ctkNetworkConnectorQtSoap.cpp
  ctkNetworkConnectorQtSoap.h
  ctkNetworkConnectorQXMLRPC.cpp
  ctkNetworkConnectorQXMLRPC.h
  ctkNetworkConnectorZeroMQ.cpp
  ctkNetworkConnectorZeroMQ.h
  ctkTopicRegistry.cpp
  ctkTopicRegistry.h
  )

set(PLUGIN_MOC_SRCS
  ctkEventBusPlugin_p.h
  ctkEventDispatcherLocal.h
  ctkEventBusManager.h
  ctkEventHandlerWrapper_p.h
  ctkEventDispatcher.h
  ctkNetworkConnectorQXMLRPC.h
  ctkNetworkConnector.h
  ctkEventDispatcherRemote.h
  ctkNetworkConnectorZeroMQ.h
  ctkNetworkConnectorQtSoap.h
  ctkEventBusImpl_p.h
  )

set(PLUGIN_UI_FORMS

  )

set(PLUGIN_resources
  #org_commontk_eventbus.qrc
)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  CACHED_RESOURCEFILES ${PLUGIN_CACHED_RESOURCEFILES}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
