PROJECT(org_commontk_slicermodule)

SET(PLUGIN_export_directive "org_commontk_slicermodule_EXPORT")

SET(PLUGIN_SRCS
  ctkSlicerModulePlugin.cpp
  ctkSlicerModulePlugin_p.h
  ctkSlicerModuleReader.cpp
  ctkSlicerModuleReader.h
  ctkSlicerModuleStringConverter.h
  ctkSlicerModuleStringConverter.cpp
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkSlicerModulePlugin_p.h
  ctkSlicerModuleReader.h
  ctkSlicerModuleStringConverter.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
  Resources/org_commontk_slicermodule.qrc
)


# Update CTK_BASE_LIBRARIES with QT libraries
IF(QT4_FOUND)
  SET(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK base libraries" FORCE)
ENDIF()

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

SET( QT_USE_QTXML ON )
SET( QT_USE_QTXMLPATTERNS ON )
INCLUDE(${QT_USE_FILE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES})

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
