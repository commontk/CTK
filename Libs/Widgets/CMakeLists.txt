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
  ctkAddRemoveComboBox.cpp
  ctkAddRemoveComboBox.h
  ctkButtonGroup.cpp
  ctkButtonGroup.h
  ctkCheckableHeaderView.cpp
  ctkCheckableHeaderView.h
  ctkCheckBoxPixmaps.cpp
  ctkCheckBoxPixmaps.h
  ctkCollapsibleButton.cpp
  ctkCollapsibleButton.h
  ctkCollapsibleGroupBox.cpp
  ctkCollapsibleGroupBox.h
  ctkColorPickerButton.cpp
  ctkColorPickerButton.h
  ctkConsoleWidget.cpp
  ctkConsoleWidget.h
  ctkCoordinatesWidget.cpp
  ctkCoordinatesWidget.h
  ctkDirectoryButton.cpp
  ctkDirectoryButton.h
  ctkDoubleRangeSlider.cpp
  ctkDoubleRangeSlider.h
  ctkDoubleSlider.cpp
  ctkDoubleSlider.h
  ctkDynamicSpacer.cpp
  ctkDynamicSpacer.h
  ctkFittedTextBrowser.cpp
  ctkFittedTextBrowser.h
  ctkMatrixWidget.cpp
  ctkMatrixWidget.h
  ctkMenuButton.cpp
  ctkMenuButton.h
  ctkRangeSlider.cpp
  ctkRangeSlider.h
  ctkRangeWidget.cpp
  ctkRangeWidget.h
  ctkSettings.cpp
  ctkSettings.h
  ctkSliderSpinBoxWidget.cpp
  ctkSliderSpinBoxWidget.h
  ctkTestApplication.cpp
  ctkTestApplication.h
  ctkTitleComboBox.cpp
  ctkTitleComboBox.h
  ctkTransferFunctionControlPointsItem.cpp
  ctkTransferFunctionControlPointsItem.h
  ctkTransferFunctionGradientItem.cpp
  ctkTransferFunctionGradientItem.h
  ctkTransferFunctionBarsItem.cpp
  ctkTransferFunctionBarsItem.h
  ctkTransferFunctionItem.cpp
  ctkTransferFunctionItem.h
  ctkTransferFunctionWidget.cpp
  ctkTransferFunctionWidget.h
  ctkTransferFunctionScene.cpp
  ctkTransferFunctionScene.h
  ctkTreeComboBox.cpp
  ctkTreeComboBox.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkAddRemoveComboBox.h
  ctkButtonGroup.h
  ctkCheckableHeaderView.h
  ctkCheckBoxPixmaps.h
  ctkCollapsibleButton.h
  ctkCollapsibleGroupBox.h
  ctkColorPickerButton.h
  ctkConsoleWidget.h
  ctkCoordinatesWidget.h
  ctkDirectoryButton.h
  ctkDoubleRangeSlider.h
  ctkDoubleSlider.h
  ctkDynamicSpacer.h
  ctkFittedTextBrowser.h
  ctkMatrixWidget.h
  ctkMenuButton.h
  ctkRangeSlider.h
  ctkRangeWidget.h
  ctkSettings.h
  ctkSliderSpinBoxWidget.h
  ctkTestApplication.h
  ctkTitleComboBox.h
  ctkTransferFunctionControlPointsItem.h
  ctkTransferFunctionGradientItem.h
  ctkTransferFunctionBarsItem.h
  ctkTransferFunctionItem.h
  ctkTransferFunctionControlPointsItem.h
  ctkTransferFunctionWidget.h
  ctkTransferFunctionScene.h
  ctkTreeComboBox.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/ctkAddRemoveComboBox.ui
  Resources/UI/ctkRangeWidget.ui
  Resources/UI/ctkSliderSpinBoxWidget.ui
  )

# Resources
SET(KIT_resources
  Resources/CTKWidgets.qrc
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
