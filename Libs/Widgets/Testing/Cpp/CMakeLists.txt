SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cxx
  ctkActionsWidgetTest1.cpp
  ctkAddRemoveComboBoxTest1.cpp
  ctkAxesWidgetTest1.cpp
  ctkButtonGroupTest1.cpp
  ctkCheckBoxPixmapsTest1.cpp
  ctkCheckableHeaderViewTest1.cpp
  ctkCheckablePushButtonTest1.cpp
  ctkCollapsibleButtonTest1.cpp
  ctkCollapsibleGroupBoxTest1.cpp
  ctkColorDialogTest1.cpp
  ctkColorDialogTest2.cpp
  ctkColorPickerButtonTest1.cpp
  ctkComboBoxTest1.cpp
  ctkConfirmExitDialogTest1.cpp
  ctkConsoleWidgetTest1.cpp
  ctkCoordinatesWidgetTest1.cpp
  ctkDirectoryButtonTest1.cpp
  ctkDoubleRangeSliderTest1.cpp
  ctkDoubleRangeSliderTest2.cpp
  ctkDoubleSliderTest1.cpp
  ctkDynamicSpacerTest1.cpp
  ctkDynamicSpacerTest2.cpp
  ctkFileDialogTest1.cpp
  ctkFittedTextBrowserTest1.cpp
  ctkFontButtonTest1.cpp
  ctkMaterialPropertyPreviewLabelTest1.cpp
  ctkMaterialPropertyWidgetTest1.cpp
  ctkMatrixWidgetTest1.cpp
  ctkMatrixWidgetTest2.cpp
  ctkMenuButtonTest1.cpp
  ctkPathLineEditTest1.cpp
  ctkRangeSliderTest1.cpp
  ctkRangeWidgetTest1.cpp
  ctkRangeWidgetTest2.cpp
  ctkDateRangeWidgetTest1.cpp
  ctkSettingsPanelTest1.cpp
  ctkSettingsTest1.cpp
  ctkSettingsDialogTest1.cpp
  ctkSliderWidgetTest1.cpp
  ctkToolTipTrapperTest1.cpp
  ctkTreeComboBoxTest1.cpp
  ctkWorkflowWidgetTest1.cpp
  ctkWorkflowWidgetTest2.cpp
  ctkExampleUseOfWorkflowWidgetUsingDerivedSteps.cpp
  ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

SET(Tests_SRCS
  ctkExampleDerivedWorkflowWidgetStep.cpp
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.cpp
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  )

SET(Tests_MOC_SRCS
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  )

SET(Tests_MOC_CPP)
QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})

ADD_EXECUTABLE(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
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
SIMPLE_TEST( ctkCheckableHeaderViewTest1 )
SIMPLE_TEST( ctkCheckablePushButtonTest1 )
SIMPLE_TEST( ctkCollapsibleButtonTest1 )
SIMPLE_TEST( ctkCollapsibleGroupBoxTest1 )
SIMPLE_TEST( ctkColorDialogTest1 )
SIMPLE_TEST( ctkColorDialogTest2 )
SIMPLE_TEST( ctkColorPickerButtonTest1 )
SIMPLE_TEST( ctkComboBoxTest1 )
SIMPLE_TEST( ctkConfirmExitDialogTest1 )
SIMPLE_TEST( ctkConsoleWidgetTest1 )
SIMPLE_TEST( ctkCoordinatesWidgetTest1 )
SIMPLE_TEST( ctkDateRangeWidgetTest1 )
SIMPLE_TEST( ctkDirectoryButtonTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest2 )
SIMPLE_TEST( ctkDoubleSliderTest1 )
SIMPLE_TEST( ctkDynamicSpacerTest1 )
SIMPLE_TEST( ctkDynamicSpacerTest2 )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingDerivedSteps )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots )
SIMPLE_TEST( ctkFileDialogTest1 )
SIMPLE_TEST( ctkFittedTextBrowserTest1 )
SIMPLE_TEST( ctkFontButtonTest1 )
SIMPLE_TEST( ctkMaterialPropertyPreviewLabelTest1 )
SIMPLE_TEST( ctkMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkMatrixWidgetTest1 )
SIMPLE_TEST( ctkMatrixWidgetTest2 )
SIMPLE_TEST( ctkMenuButtonTest1 )
SIMPLE_TEST( ctkPathLineEditTest1 )
SIMPLE_TEST( ctkRangeSliderTest1 )
SIMPLE_TEST( ctkRangeWidgetTest1 )
SIMPLE_TEST( ctkRangeWidgetTest2 )
SIMPLE_TEST( ctkSettingsDialogTest1 )
SIMPLE_TEST( ctkSettingsPanelTest1 )
SIMPLE_TEST( ctkSettingsTest1 )
SIMPLE_TEST( ctkSliderWidgetTest1 )
SIMPLE_TEST( ctkToolTipTrapperTest1 )
SIMPLE_TEST( ctkTreeComboBoxTest1 )
SIMPLE_TEST( ctkWorkflowWidgetTest1 )
SIMPLE_TEST( ctkWorkflowWidgetTest2 )
