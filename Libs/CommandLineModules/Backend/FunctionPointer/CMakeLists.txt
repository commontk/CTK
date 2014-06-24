project(CTKCommandLineModulesBackendFunctionPointer)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_CMDLINEMODULEBACKENDFP_EXPORT")

# Additional directories to include

# Source files
set(KIT_SRCS
  ctkCmdLineModuleBackendFPTypeTraits.h
  ctkCmdLineModuleBackendFPUtil.cpp
  ctkCmdLineModuleBackendFPUtil_p.h
  ctkCmdLineModuleBackendFunctionPointer.cpp
  ctkCmdLineModuleBackendFPDescriptionPrivate.cpp
  ctkCmdLineModuleFunctionPointerTask.cpp
  ctkCmdLineModuleFunctionPointerTask_p.h
)

# Headers that should run through moc
set(KIT_MOC_SRCS
)

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
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
  #add_subdirectory(Testing)
endif()
