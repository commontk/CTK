PROJECT(CTKWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_WIDGETS_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  ctkColorPickerButton.cpp
  ctkColorPickerButton.h
  ctkMatrixWidget.cpp
  ctkMatrixWidget.h
  ctkSettings.cpp
  ctkSettings.h
  ctkTransferFunctionItems.cpp
  ctkTransferFunctionItems.h
  ctkTransferFunctionWidget.cpp
  ctkTransferFunctionWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkColorPickerButton.h
  ctkMatrixWidget.h
  ctkSettings.h
  ctkTransferFunctionItems.h
  ctkTransferFunctionWidget.h
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

# Plugins
ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
