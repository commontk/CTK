project(org_commontk_eventadmin)

set(PLUGIN_export_directive "org_commontk_eventadmin_EXPORT")

set(PLUGIN_SRCS
  ctkEAConfiguration_p.h
  ctkEAConfiguration.cpp
  ctkEAMetaTypeProvider_p.h
  ctkEAMetaTypeProvider.cpp
  ctkEventAdminActivator.cpp
  ctkEventAdminActivator_p.h
  ctkEventAdminImpl_p.h
  ctkEventAdminImpl.tpp
  ctkEventAdminService_p.h
  ctkEventAdminService.cpp

  adapter/ctkEAAbstractAdapter_p.h
  adapter/ctkEAAbstractAdapter.cpp
  adapter/ctkEAFrameworkEventAdapter_p.h
  adapter/ctkEAFrameworkEventAdapter.cpp
  adapter/ctkEALogEventAdapter_p.h
  adapter/ctkEALogEventAdapter.cpp
  adapter/ctkEAPluginEventAdapter_p.h
  adapter/ctkEAPluginEventAdapter.cpp
  adapter/ctkEAServiceEventAdapter_p.h
  adapter/ctkEAServiceEventAdapter.cpp

  dispatch/ctkEAChannel_p.h
  dispatch/ctkEADefaultThreadPool_p.h
  dispatch/ctkEADefaultThreadPool.cpp
  dispatch/ctkEAInterruptibleThread_p.h
  dispatch/ctkEAInterruptibleThread.cpp
  dispatch/ctkEALinkedQueue_p.h
  dispatch/ctkEALinkedQueue.cpp
  dispatch/ctkEAPooledExecutor_p.h
  dispatch/ctkEAPooledExecutor.cpp
  dispatch/ctkEASignalPublisher_p.h
  dispatch/ctkEASignalPublisher.cpp
  dispatch/ctkEASyncMasterThread_p.h
  dispatch/ctkEASyncMasterThread.cpp
  dispatch/ctkEAThreadFactory_p.h
  dispatch/ctkEAThreadFactoryUser.cpp
  dispatch/ctkEAThreadFactoryUser_p.h
  dispatch/ctkEAInterruptedException_p.h
  dispatch/ctkEAInterruptedException.cpp

  handler/ctkEABlackList_p.h
  handler/ctkEABlacklistingHandlerTasks_p.h
  handler/ctkEABlacklistingHandlerTasks.tpp
  handler/ctkEACacheFilters_p.h
  handler/ctkEACacheFilters.tpp
  handler/ctkEACacheTopicHandlerFilters_p.h
  handler/ctkEACacheTopicHandlerFilters.tpp
  handler/ctkEACleanBlackList.cpp
  handler/ctkEACleanBlackList_p.h
  handler/ctkEAFilters_p.h
  handler/ctkEAHandlerTasks_p.h
  handler/ctkEASlotHandler_p.h
  handler/ctkEASlotHandler.cpp
  handler/ctkEATopicHandlerFilters_p.h

  tasks/ctkEAAsyncDeliverTasks_p.h
  tasks/ctkEAAsyncDeliverTasks.tpp
  tasks/ctkEADeliverTask_p.h
  tasks/ctkEAHandlerTask_p.h
  tasks/ctkEAHandlerTask.tpp
  tasks/ctkEASyncDeliverTasks_p.h
  tasks/ctkEASyncDeliverTasks.tpp
  tasks/ctkEASyncThread.cpp
  tasks/ctkEASyncThread_p.h

  util/ctkEABrokenBarrierException.cpp
  util/ctkEABrokenBarrierException_p.h
  util/ctkEACacheMap_p.h
  util/ctkEACyclicBarrier.cpp
  util/ctkEACyclicBarrier_p.h
  util/ctkEALeastRecentlyUsedCacheMap_p.h
  util/ctkEALeastRecentlyUsedCacheMap.tpp
  util/ctkEALogTracker.cpp
  util/ctkEALogTracker_p.h
  util/ctkEARendezvous.cpp
  util/ctkEARendezvous_p.h
  util/ctkEATimeoutException.cpp
  util/ctkEATimeoutException_p.h
)

set(PLUGIN_MOC_SRCS
  adapter/ctkEAFrameworkEventAdapter_p.h
  adapter/ctkEALogEventAdapter_p.h
  adapter/ctkEAPluginEventAdapter_p.h
  adapter/ctkEAServiceEventAdapter_p.h

  dispatch/ctkEAInterruptibleThread_p.h
  dispatch/ctkEASignalPublisher_p.h
  dispatch/ctkEASyncMasterThread_p.h

  handler/ctkEASlotHandler_p.h

  tasks/ctkEASyncThread_p.h

  ctkEAConfiguration_p.h
  ctkEAMetaTypeProvider_p.h
  ctkEventAdminActivator_p.h
  ctkEventAdminService_p.h
)

set(PLUGIN_resources

)

set(PLUGIN_CACHED_RESOURCEFILES

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

if(UNIX AND NOT APPLE)
  list(APPEND PLUGIN_target_libraries rt)
endif()

ctkMacroBuildPlugin(
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  CACHED_RESOURCEFILES ${PLUGIN_CACHED_RESOURCEFILES}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
