project(CTKCommandLineModulesFrontendQtGui)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_CMDLINEMODULEQTGUI_EXPORT")

# Additional directories to include

# Source files
set(KIT_SRCS
  ctkCmdLineModuleFrontendFactoryQtGui.cpp
  ctkCmdLineModuleFrontendQtGui.cpp
  ctkCmdLineModuleQtComboBox.cpp
  ctkCmdLineModuleQtComboBox_p.h
  ctkCmdLineModuleQtUiLoader.cpp
  ctkCmdLineModuleObjectTreeWalker_p.h
  ctkCmdLineModuleObjectTreeWalker.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkCmdLineModuleQtComboBox_p.h
  ctkCmdLineModuleQtUiLoader.h
)

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkCmdLineModulesFrontendQtGui.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::UiTools Qt5::Widgets)
else()
  set(QT_USE_QTUITOOLS 1)
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
