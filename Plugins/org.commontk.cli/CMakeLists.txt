

SET(plugin_srcs
  ctkCLIPlugin.cxx
)

SET(plugin_moc
  ctkCLIPlugin.h
)

SET(plugin_qrc_files
  org_commontk_cli.qrc
)

SET(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/Plugins)

CtkMacroBuildQtPlugin(
  NAME org_commontk_cli
  SRCS ${plugin_srcs}
  MOC_SRCS ${plugin_moc}
  QRC_FILES ${plugin_qrc_files}
  EXPORT_DIRECTIVE "org_commontk_cli_EXPORT"
)