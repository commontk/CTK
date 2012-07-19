project(CTKCommandLineModulesCore)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_CMDLINEMODULECORE_EXPORT")

# Additional directories to include

# Source files
set(KIT_SRCS
  ctkCmdLineModuleDefaultPathBuilder.cpp
  ctkCmdLineModuleDescription.cpp
  ctkCmdLineModuleDescriptionPrivate.h
  ctkCmdLineModuleDirectoryWatcher.cpp
  ctkCmdLineModuleInstance.cpp
  ctkCmdLineModuleInstanceFactory.cpp
  ctkCmdLineModuleManager.cpp
  ctkCmdLineModuleParameter.cpp
  ctkCmdLineModuleParameterPrivate.cpp
  ctkCmdLineModuleParameterGroup.cpp
  ctkCmdLineModuleParameterGroupPrivate.h
  ctkCmdLineModuleParameterParsers_p.h
  ctkCmdLineModulePathBuilder.cpp
  ctkCmdLineModuleProcessTask.cpp
  ctkCmdLineModuleXmlProgressWatcher.cpp
  ctkCmdLineModuleReference.cpp
  ctkCmdLineModuleReferencePrivate.cpp
  ctkCmdLineModuleRunException.cpp
  ctkCmdLineModuleXmlException.cpp
  ctkCmdLineModuleXmlMsgHandler_p.h
  ctkCmdLineModuleXmlMsgHandler.cpp
  ctkCmdLineModuleXmlParser_p.h
  ctkCmdLineModuleXmlParser.cpp
  ctkCmdLineModuleXmlValidator.cpp
  ctkCmdLineModuleXslTransform.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkCmdLineModuleDirectoryWatcher.h
  ctkCmdLineModuleDirectoryWatcherPrivate.h
  ctkCmdLineModuleManager.h
  ctkCmdLineModuleInstance.h
  ctkCmdLineModuleProcessTask.h
)

qt4_wrap_cpp(_dummy ctkCmdLineModuleXmlProgressWatcher.h)
set_source_files_properties(ctkCmdLineModuleXmlProgressWatcher.cpp
  PROPERTIES OBJECT_DEPENDS ${_dummy})

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkCmdLineModules.qrc
)

set(QT_USE_QTXMLPATTERNS 1)
include(${QT_USE_FILE})

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

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

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(CTK_WRAP_PYTHONQT_FULL OR CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
