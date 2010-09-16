PROJECT(CTKModuleDescription)

#
# 3rd party dependencies
#

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_MODULDESC_EXPORT")

# Additional directories to include
  
# Source files
SET(KIT_SRCS
  ctkModuleDescription.h
  ctkModuleDescription.cpp
  ctkModuleParameter.h
  ctkModuleParameter.cpp
  ctkModuleParameterGroup.h
  ctkModuleParameterGroup.cpp
  ctkModuleDescriptionReaderInterface.h
  ctkModuleDescriptionReader.h
  ctkModuleDescriptionReader.cpp
  ctkModuleDescriptionConverterInterface.h
  ctkModuleDescriptionConverter.h
  ctkModuleDescriptionConverter.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkModuleDescriptionReaderInterface.h
  ctkModuleDescriptionReader.h
  ctkModuleDescriptionConverterInterface.h
  ctkModuleDescriptionConverter.h
)

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
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

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
