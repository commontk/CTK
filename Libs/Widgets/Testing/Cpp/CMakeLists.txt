SET(KIT ${PROJECT_NAME})

SET(TEST_SOURCES
  ctkActionsWidgetTest1.cpp
  ctkAddRemoveComboBoxTest1.cpp
  ctkAxesWidgetTest1.cpp
  ctkButtonGroupTest1.cpp
  ctkCheckBoxPixmapsTest1.cpp
  ctkCheckBoxTest1.cpp
  ctkCheckableComboBoxTest1.cpp
  ctkCheckableHeaderViewTest1.cpp
  ctkCheckableHeaderViewTest2.cpp
  ctkCheckableModelHelperTest2.cpp
  ctkCheckablePushButtonTest1.cpp
  ctkCollapsibleButtonTest1.cpp
  ctkCollapsibleButtonTest2.cpp
  ctkCollapsibleGroupBoxTest1.cpp
  ctkColorDialogTest1.cpp
  ctkColorDialogTest2.cpp
  ctkColorPickerButtonTest1.cpp
  ctkComboBoxTest1.cpp
  ctkCompleterTest1.cpp
  ctkConfirmExitDialogTest1.cpp
  ctkConsoleTest1.cpp
  ctkCoordinatesWidgetTest1.cpp
  ctkCrosshairLabelTest1.cpp
  ctkDirectoryButtonTest1.cpp
  ctkDoubleRangeSliderTest1.cpp
  ctkDoubleRangeSliderTest2.cpp
  ctkDoubleSliderTest1.cpp
  ctkDoubleSliderTest2.cpp
  ctkDynamicSpacerTest1.cpp
  ctkDynamicSpacerTest2.cpp
  ctkErrorLogWidgetTest1.cpp
  ctkExpandButtonTest1.cpp
  ctkFileDialogTest1.cpp
  ctkFittedTextBrowserTest1.cpp
  ctkFlowLayoutTest1.cpp
  ctkFontButtonTest1.cpp
  ctkLayoutManagerTest1.cpp
  ctkMaterialPropertyPreviewLabelTest1.cpp
  ctkMaterialPropertyWidgetTest1.cpp
  ctkMaterialPropertyWidgetTest2.cpp
  ctkMatrixWidgetTest1.cpp
  ctkMatrixWidgetTest2.cpp
  ctkMenuButtonTest1.cpp
  ctkMenuComboBoxTest1.cpp
  ctkMenuComboBoxTest2.cpp
  ctkMenuComboBoxTest3.cpp
  ctkModalityWidgetTest1.cpp
  ctkPathLineEditTest1.cpp
  ctkPopupWidgetTest1.cpp
  ctkRangeSliderTest1.cpp
  ctkRangeWidgetTest1.cpp
  ctkRangeWidgetTest2.cpp
  ctkDateRangeWidgetTest1.cpp
  ctkScreenshotDialogTest1.cpp
  ctkSearchBoxTest1.cpp
  ctkSearchBoxTest2.cpp
  ctkSettingsPanelTest1.cpp
  ctkSettingsPanelTest2.cpp
  ctkSettingsTest1.cpp
  ctkSettingsDialogTest1.cpp
  ctkSignalMapperTest1.cpp
  ctkSliderWidgetTest1.cpp
  ctkSliderWidgetTest2.cpp
  ctkThumbnailListWidgetTest1.cpp
  ctkThumbnailLabelTest1.cpp
  ctkToolTipTrapperTest1.cpp
  ctkTreeComboBoxTest1.cpp
  ctkWidgetsUtilsTest1.cpp
  ctkWidgetsUtilsTestGrabWidget.cpp
  ctkWorkflowWidgetTest1.cpp
  ctkWorkflowWidgetTest2.cpp
  ctkExampleUseOfWorkflowWidgetUsingDerivedSteps.cpp
  ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots.cpp
  )

#
# Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  LIST(APPEND TEST_SOURCES
    ctkCrosshairLabelTest2.cpp
    )
ENDIF()

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cxx
  ${TEST_SOURCES}
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

SET(Tests_SRCS
  ctkExampleDerivedWorkflowWidgetStep.cpp
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.cpp
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  ctkSettingsPanelTest2Helper.cpp
  ctkSettingsPanelTest2Helper.h
  )

SET(Tests_MOC_SRCS
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  ctkSettingsPanelTest2Helper.h
  )

SET(Tests_MOC_CPP)
QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})

ADD_EXECUTABLE(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME}
            -D "${CTKData_DIR}/Data"
            -V "${CTKData_DIR}/Baseline/Libs/Widgets"
            )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkActionsWidgetTest1 )
SIMPLE_TEST( ctkAddRemoveComboBoxTest1 )
SIMPLE_TEST( ctkAxesWidgetTest1 )
SIMPLE_TEST( ctkButtonGroupTest1 )
SIMPLE_TEST( ctkCheckBoxPixmapsTest1 )
SIMPLE_TEST( ctkCheckBoxTest1 )
SIMPLE_TEST( ctkCheckableComboBoxTest1 )
SIMPLE_TEST( ctkCheckableHeaderViewTest1 )
SIMPLE_TEST( ctkCheckableHeaderViewTest2 )
SIMPLE_TEST( ctkCheckableModelHelperTest2 )
SIMPLE_TEST( ctkCheckablePushButtonTest1 )
SIMPLE_TEST( ctkCollapsibleButtonTest1 )
SIMPLE_TEST( ctkCollapsibleButtonTest2 )
SIMPLE_TEST( ctkCollapsibleGroupBoxTest1 )
SIMPLE_TEST( ctkColorDialogTest1 )
SIMPLE_TEST( ctkColorDialogTest2 )
SIMPLE_TEST( ctkColorPickerButtonTest1 )
SIMPLE_TEST( ctkComboBoxTest1 )
SIMPLE_TEST( ctkCompleterTest1 )
SIMPLE_TEST( ctkConfirmExitDialogTest1 )
SIMPLE_TEST( ctkConsoleTest1 )
SIMPLE_TEST( ctkCoordinatesWidgetTest1 )
SIMPLE_TEST( ctkCrosshairLabelTest1 )
SIMPLE_TEST( ctkDateRangeWidgetTest1 )
SIMPLE_TEST( ctkDirectoryButtonTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest2 )
SIMPLE_TEST( ctkDoubleSliderTest1 )
SIMPLE_TEST( ctkDoubleSliderTest2 )
SIMPLE_TEST( ctkDynamicSpacerTest1 )
SIMPLE_TEST( ctkDynamicSpacerTest2 )
SIMPLE_TEST( ctkErrorLogWidgetTest1 )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingDerivedSteps )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots )
SIMPLE_TEST( ctkExpandButtonTest1 )
SIMPLE_TEST( ctkFileDialogTest1 )
SIMPLE_TEST( ctkFittedTextBrowserTest1 )
SIMPLE_TEST( ctkFlowLayoutTest1 )
SIMPLE_TEST( ctkFontButtonTest1 )
SIMPLE_TEST( ctkLayoutManagerTest1 )
SIMPLE_TEST( ctkMaterialPropertyPreviewLabelTest1 )
SIMPLE_TEST( ctkMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkMaterialPropertyWidgetTest2 )
SIMPLE_TEST( ctkMatrixWidgetTest1 )
SIMPLE_TEST( ctkMatrixWidgetTest2 )
SIMPLE_TEST( ctkMenuButtonTest1 )
SIMPLE_TEST( ctkMenuComboBoxTest1 )
SIMPLE_TEST( ctkMenuComboBoxTest2 )
SIMPLE_TEST( ctkMenuComboBoxTest3 )
SIMPLE_TEST( ctkModalityWidgetTest1 )
SIMPLE_TEST( ctkPathLineEditTest1 )
SIMPLE_TEST( ctkPopupWidgetTest1 )
SIMPLE_TEST( ctkRangeSliderTest1 )
SIMPLE_TEST( ctkRangeWidgetTest1 )
SIMPLE_TEST( ctkRangeWidgetTest2 )
SIMPLE_TEST( ctkScreenshotDialogTest1 )
SIMPLE_TEST( ctkSearchBoxTest1 )
SIMPLE_TEST( ctkSearchBoxTest2 )
SIMPLE_TEST( ctkSettingsDialogTest1 )
SIMPLE_TEST( ctkSettingsPanelTest1 )
SIMPLE_TEST( ctkSettingsPanelTest2 )
SIMPLE_TEST( ctkSettingsTest1 )
SIMPLE_TEST( ctkSignalMapperTest1 )
SIMPLE_TEST( ctkSliderWidgetTest1 )
SIMPLE_TEST( ctkSliderWidgetTest2 )
SIMPLE_TEST( ctkThumbnailListWidgetTest1 )
SIMPLE_TEST( ctkThumbnailLabelTest1 )
SIMPLE_TEST( ctkToolTipTrapperTest1 )
SIMPLE_TEST( ctkTreeComboBoxTest1 )
SIMPLE_TEST( ctkWidgetsUtilsTest1 )
SIMPLE_TEST( ctkWidgetsUtilsTestGrabWidget )
SIMPLE_TEST( ctkWorkflowWidgetTest1 )
SIMPLE_TEST( ctkWorkflowWidgetTest2 )

#
# Add Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  SIMPLE_TEST( ctkCrosshairLabelTest2 )
ENDIF()
