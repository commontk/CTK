PROJECT(CTKWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_WIDGETS_EXPORT")


# Source files
SET(KIT_SRCS
  ctkWorkflowAbstractWidgetStep.cpp
  ctkWorkflowAbstractWidgetStep.h
  ctkWorkflowAbstractWidgetStep_p.h
  ctkActionsWidget.cpp
  ctkActionsWidget.h
  ctkAddRemoveComboBox.cpp
  ctkAddRemoveComboBox.h
  ctkAxesWidget.cpp
  ctkAxesWidget.h
  ctkButtonGroup.cpp
  ctkButtonGroup.h
  ctkCheckableHeaderView.cpp
  ctkCheckableHeaderView.h
  ctkCheckBoxPixmaps.cpp
  ctkCheckBoxPixmaps.h
  ctkCheckablePushButton.cpp
  ctkCheckablePushButton.h
  ctkComboBox.cpp
  ctkComboBox.h
  ctkCollapsibleButton.cpp
  ctkCollapsibleButton.h
  ctkCollapsibleGroupBox.cpp
  ctkCollapsibleGroupBox.h
  ctkColorDialog.cpp
  ctkColorDialog.h
  ctkColorPickerButton.cpp
  ctkColorPickerButton.h
  ctkConsoleWidget.cpp
  ctkConsoleWidget.h
  ctkConfirmExitDialog.cpp
  ctkConfirmExitDialog.h
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
  ctkFileDialog.cpp
  ctkFileDialog.h
  ctkFittedTextBrowser.cpp
  ctkFittedTextBrowser.h
  ctkFontButton.cpp
  ctkFontButton.h
  ctkIconEnginePlugin.cpp
  ctkIconEnginePlugin.h
  ctkMaterialPropertyPreviewLabel.cpp
  ctkMaterialPropertyPreviewLabel.h
  ctkMaterialPropertyWidget.cpp
  ctkMaterialPropertyWidget.h
  ctkMatrixWidget.cpp
  ctkMatrixWidget.h
  ctkMenuButton.cpp
  ctkMenuButton.h
  ctkPathLineEdit.cpp
  ctkPathLineEdit.h
  ctkPixmapIconEngine.cpp
  ctkPixmapIconEngine.h
  ctkRangeSlider.cpp
  ctkRangeSlider.h
  ctkRangeWidget.cpp
  ctkRangeWidget.h
  ctkScreenshotDialog.cpp
  ctkScreenshotDialog.h
  ctkScreenshotDialog_p.h
  ctkSettings.cpp
  ctkSettings.h
  ctkSettingsDialog.cpp
  ctkSettingsDialog.h
  ctkSettingsPanel.cpp
  ctkSettingsPanel.h
  ctkSliderWidget.cpp
  ctkSliderWidget.h
  ctkTestApplication.cpp
  ctkTestApplication.h
  ctkToolTipTrapper.cpp
  ctkToolTipTrapper.h
  ctkTransferFunctionBarsItem.cpp
  ctkTransferFunctionBarsItem.h
  ctkTransferFunctionControlPointsItem.cpp
  ctkTransferFunctionControlPointsItem.h
  ctkTransferFunctionGradientItem.cpp
  ctkTransferFunctionGradientItem.h
  ctkTransferFunctionItem.cpp
  ctkTransferFunctionItem.h
  ctkTransferFunctionScene.cpp
  ctkTransferFunctionScene.h
  ctkTransferFunctionView.cpp
  ctkTransferFunctionView.h
  ctkTreeComboBox.cpp
  ctkTreeComboBox.h
  ctkWorkflowAbstractPagedWidget.cpp
  ctkWorkflowAbstractPagedWidget.h
  ctkWorkflowButtonBoxWidget.cpp
  ctkWorkflowButtonBoxWidget.h
  ctkWorkflowGroupBox.cpp
  ctkWorkflowGroupBox.h
  ctkWorkflowStackedWidget.cpp
  ctkWorkflowStackedWidget.h
  ctkWorkflowTabWidget.cpp
  ctkWorkflowTabWidget.h
  ctkWorkflowWidget.cpp
  ctkWorkflowWidget.h
  ctkWorkflowWidgetStep.cpp
  ctkWorkflowWidgetStep.h
  ctkDateRangeWidget.cpp
  ctkDateRangeWidget.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkWorkflowAbstractWidgetStep_p.h
  ctkActionsWidget.h
  ctkAxesWidget.h
  ctkAddRemoveComboBox.h
  ctkButtonGroup.h
  ctkCheckableHeaderView.h
  ctkCheckablePushButton.h
  ctkCheckBoxPixmaps.h
  ctkComboBox.h
  ctkCollapsibleButton.h
  ctkCollapsibleGroupBox.h
  ctkColorDialog.h
  ctkColorPickerButton.h
  ctkConfirmExitDialog.h
  ctkConsoleWidget.h
  ctkCoordinatesWidget.h
  ctkDirectoryButton.h
  ctkDoubleRangeSlider.h
  ctkDoubleSlider.h
  ctkDynamicSpacer.h
  ctkFileDialog.h
  ctkFittedTextBrowser.h
  ctkFontButton.h
  ctkIconEnginePlugin.h
  ctkMaterialPropertyPreviewLabel.h
  ctkMaterialPropertyWidget.h
  ctkMatrixWidget.h
  ctkMenuButton.h
  ctkPathLineEdit.h
  ctkRangeSlider.h
  ctkRangeWidget.h
  ctkScreenshotDialog.h
  ctkScreenshotDialog_p.h
  ctkSettings.h
  ctkSettingsDialog.h
  ctkSettingsPanel.h
  ctkSliderWidget.h
  ctkTestApplication.h
  ctkToolTipTrapper.h
  ctkTransferFunctionBarsItem.h
  ctkTransferFunctionControlPointsItem.h
  ctkTransferFunctionGradientItem.h
  ctkTransferFunctionItem.h
  ctkTransferFunctionScene.h
  ctkTransferFunctionView.h
  ctkTreeComboBox.h
  ctkWorkflowAbstractPagedWidget.h
  ctkWorkflowButtonBoxWidget.h
  ctkWorkflowGroupBox.h
  ctkWorkflowStackedWidget.h
  ctkWorkflowTabWidget.h
  ctkWorkflowWidget.h
  ctkWorkflowWidgetStep.h
  ctkDateRangeWidget.h
  )

# UI files
SET(KIT_UI_FORMS
  Resources/UI/ctkAddRemoveComboBox.ui
  Resources/UI/ctkRangeWidget.ui
  Resources/UI/ctkDateRangeWidget.ui
  Resources/UI/ctkMaterialPropertyWidget.ui
  Resources/UI/ctkScreenshotDialog.ui
  Resources/UI/ctkSettingsDialog.ui
  Resources/UI/ctkSliderWidget.ui
  Resources/UI/ctkWorkflowGroupBox.ui
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
