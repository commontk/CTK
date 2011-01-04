PROJECT(org_commontk_qtmobility_service)

SET(PLUGIN_export_directive "org_commontk_qtmobsfw_EXPORT")

SET(PLUGIN_SRCS
  ctkQtMobilityServiceActivator.cpp
  ctkQtMobilityServiceConstants.cpp
  ctkQtMobilityServiceFactory.cpp
  ctkQtMobilityServiceRuntime.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkQtMobilityServiceActivator_p.h
  ctkQtMobilityServiceFactory_p.h
  ctkQtMobilityServiceRuntime_p.h
)

SET(PLUGIN_resources

)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
