project(CTKModuleDescription)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_MODULDESC_EXPORT")

# Additional directories to include

# Source files
set(KIT_SRCS
  ctkModuleDescription.h
  ctkModuleDescription.cpp
  ctkModuleParameter.h
  ctkModuleParameter.cpp
  ctkModuleParameterGroup.h
  ctkModuleParameterGroup.cpp
  ctkModuleDescriptionReaderInterface.cpp
  ctkModuleDescriptionReaderInterface.h
  ctkModuleDescriptionReader.h
  ctkModuleDescriptionReader.cpp
  ctkModuleDescriptionConverterInterface.h
  ctkModuleDescriptionConverter.h
  ctkModuleDescriptionConverter.cpp
  ctkModuleDescriptionExecutionInterface.h
  ctkModuleDescriptionExecution.h
  ctkModuleDescriptionExecution.cpp
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkModuleDescriptionReaderInterface.h
  ctkModuleDescriptionReader.h
  ctkModuleDescriptionConverterInterface.h
  ctkModuleDescriptionConverter.h
  ctkModuleDescriptionExecutionInterface.h
  ctkModuleDescriptionExecution.h
)

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

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
