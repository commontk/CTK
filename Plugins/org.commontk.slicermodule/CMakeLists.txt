project(org_commontk_slicermodule)

set(PLUGIN_export_directive "org_commontk_slicermodule_EXPORT")

set(PLUGIN_SRCS
  ctkSlicerModulePlugin.cpp
  ctkSlicerModulePlugin_p.h
  ctkSlicerModuleReader.cpp
  ctkSlicerModuleReader.h
  ctkSlicerModuleStringConverter.h
  ctkSlicerModuleStringConverter.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkSlicerModulePlugin_p.h
  ctkSlicerModuleReader.h
  ctkSlicerModuleStringConverter.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
  Resources/org_commontk_slicermodule.qrc
)


# Update CTK_BASE_LIBRARIES with QT libraries
if(QT4_FOUND)
  set(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK base libraries" FORCE)
endif()

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

set( QT_USE_QTXML ON )
set( QT_USE_QTXMLPATTERNS ON )
include(${QT_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
