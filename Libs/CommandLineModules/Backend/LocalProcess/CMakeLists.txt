project(CTKCommandLineModulesBackendLocalProcess)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_CMDLINEMODULEBACKENDLP_EXPORT")

# Additional directories to include

# Source files
set(KIT_SRCS
  ctkCmdLineModuleBackendLocalProcess.cpp
  ctkCmdLineModuleProcessTask.cpp
  ctkCmdLineModuleProcessWatcher.cpp
  ctkCmdLineModuleProcessWatcher_p.h
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkCmdLineModuleProcessWatcher_p.h
)

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkCmdLineModulesBackendLocalProcess.qrc
)

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

find_package(Doxygen)
if(DOXYGEN_FOUND)
  # Copy XML schema file to the Doxygen output directory
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ctkCmdLineModuleProcess.xsd
                                                  ${CTK_BINARY_DIR}/Documentation/html/ctkCmdLineModuleProcess.xsd)
endif()

# Testing
if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif()
