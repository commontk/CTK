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
  ctkCmdLineModuleBackend.cpp
  ctkCmdLineModuleCache.cpp
  ctkCmdLineModuleCache_p.h
  ctkCmdLineModuleConcurrentHelpers.cpp
  ctkCmdLineModuleDefaultPathBuilder.cpp
  ctkCmdLineModuleDescription.cpp
  ctkCmdLineModuleDescription_p.h
  ctkCmdLineModuleDirectoryWatcher.cpp
  ctkCmdLineModuleDirectoryWatcher_p.h
  ctkCmdLineModuleFrontend.h
  ctkCmdLineModuleFrontend.cpp
  ctkCmdLineModuleFrontendFactory.cpp
  ctkCmdLineModuleFuture.cpp
  ctkCmdLineModuleFutureInterface_p.h
  ctkCmdLineModuleFutureInterface.cpp
  ctkCmdLineModuleFutureWatcher.cpp
  ctkCmdLineModuleManager.cpp
  ctkCmdLineModuleParameter.cpp
  ctkCmdLineModuleParameter_p.h
  ctkCmdLineModuleParameterGroup.cpp
  ctkCmdLineModuleParameterGroup_p.h
  ctkCmdLineModuleParameterParsers_p.h
  ctkCmdLineModulePathBuilder.cpp
  ctkCmdLineModuleResult.cpp
  ctkCmdLineModuleXmlProgressWatcher.h
  ctkCmdLineModuleXmlProgressWatcher.cpp
  ctkCmdLineModuleReference.cpp
  ctkCmdLineModuleRunException.cpp
  ctkCmdLineModuleTimeoutException.cpp
  ctkCmdLineModuleUtils.cpp
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
  ctkCmdLineModuleDirectoryWatcher_p.h
  ctkCmdLineModuleFutureWatcher.h
  ctkCmdLineModuleManager.h
)

set(KIT_GENERATE_MOC_SRCS
  ctkCmdLineModuleFrontend.h
  ctkCmdLineModuleXmlProgressWatcher.h
)

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkCmdLineModules.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::XmlPatterns Qt5::Widgets Qt5::Concurrent)
else()
  set(QT_USE_QTXMLPATTERNS 1)
  include(${QT_USE_FILE})
endif()

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  GENERATE_MOC_SRCS ${KIT_GENERATE_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(CTK_WRAP_PYTHONQT_LIGHT)
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
