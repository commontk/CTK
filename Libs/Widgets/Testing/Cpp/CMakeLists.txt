set(KIT ${PROJECT_NAME})

set(TEST_SOURCES
  ctkActionsWidgetTest1.cpp
  ctkAddRemoveComboBoxTest1.cpp
  ctkAxesWidgetTest1.cpp
  ctkButtonGroupTest1.cpp
  ctkCheckableModelHelperTest1.cpp
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
  ctkConsoleTest.cpp
  ctkConsoleTest1.cpp
  ctkCoordinatesWidgetTest.cpp
  ctkCoordinatesWidgetTest1.cpp
  ctkCoordinatesWidgetValueProxyTest.cpp
  ctkCrosshairLabelTest1.cpp
  ctkDirectoryButtonTest1.cpp
  ctkDoubleRangeSliderTest.cpp
  ctkDoubleRangeSliderTest1.cpp
  ctkDoubleRangeSliderTest2.cpp
  ctkDoubleRangeSliderValueProxyTest.cpp
  ctkDoubleSliderTest.cpp
  ctkDoubleSliderTest1.cpp
  ctkDoubleSliderValueProxyTest.cpp
  ctkDoubleSpinBoxTest.cpp
  ctkDoubleSpinBoxTest1.cpp
  ctkDoubleSpinBoxValueProxyTest.cpp
  ctkDynamicSpacerTest1.cpp
  ctkDynamicSpacerTest2.cpp
  ctkErrorLogFDMessageHandlerWithThreadsTest1.cpp
  ctkErrorLogModelTest1.cpp
  ctkErrorLogModelEntryGroupingTest1.cpp
  ctkErrorLogModelTerminalOutputTest1.cpp
  ctkErrorLogModelTest4.cpp
  ctkErrorLogQtMessageHandlerWithThreadsTest1.cpp
  ctkErrorLogStreamMessageHandlerWithThreadsTest1.cpp
  ctkErrorLogWidgetTest1.cpp
  ctkExpandButtonTest1.cpp
  ctkExpandableWidgetTest1.cpp
  ctkFileDialogTest1.cpp
  ctkFlatProxyModelTest.cpp
  ctkFittedTextBrowserTest1.cpp
  ctkFlowLayoutTest1.cpp
  ctkFontButtonTest.cpp
  ctkHistogramTest1.cpp
  ctkLanguageComboBoxTest.cpp
  ctkLayoutManagerTest1.cpp
  ctkMaterialPropertyPreviewLabelTest1.cpp
  ctkMaterialPropertyWidgetTest1.cpp
  ctkMaterialPropertyWidgetTest2.cpp
  ctkMatrixWidgetTest.cpp
  ctkMatrixWidgetTest1.cpp
  ctkMatrixWidgetTest2.cpp
  ctkMenuButtonTest1.cpp
  ctkMenuComboBoxTest1.cpp
  ctkMenuComboBoxTest2.cpp
  ctkMenuComboBoxTest3.cpp
  ctkMenuComboBoxTest4.cpp
  ctkMessageBoxDontShowAgainTest.cpp
  ctkModalityWidgetTest1.cpp
  ctkPathLineEditTest1.cpp
  ctkPathListWidgetTest.cpp
  ctkPathListWidgetWithButtonsTest.cpp
  ctkPopupWidgetTest1.cpp
  ctkPushButtonTest.cpp
  ctkProxyStyleTest1.cpp
  ctkRangeSliderTest.cpp
  ctkRangeSliderTest1.cpp
  ctkRangeWidgetTest.cpp
  ctkRangeWidgetTest1.cpp
  ctkRangeWidgetValueProxyTest.cpp
  ctkDateRangeWidgetTest1.cpp
  ctkScreenshotDialogTest1.cpp
  ctkSearchBoxTest.cpp
  ctkSearchBoxTest1.cpp
  ctkSearchBoxTest2.cpp
  ctkSettingsPanelTest.cpp
  ctkSettingsPanelTest1.cpp
  ctkSettingsTest1.cpp
  ctkSettingsDialogTest1.cpp
  ctkSignalMapperTest1.cpp
  ctkSliderWidgetTest.cpp
  ctkSliderWidgetTest1.cpp
  ctkSliderWidgetTest2.cpp
  ctkSliderWidgetValueProxyTest.cpp
  ctkThumbnailListWidgetTest1.cpp
  ctkThumbnailLabelTest1.cpp
  ctkToolTipTrapperTest1.cpp
  ctkTransferFunctionTest1.cpp
  ctkTransferFunctionRepresentationTest1.cpp
  ctkTransferFunctionRepresentationTest2.cpp
  ctkTreeComboBoxTest1.cpp
  ctkWidgetsTestingUtilitiesTest.cpp
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
if(EXISTS "${CTKData_DIR}")
  list(APPEND TEST_SOURCES
    ctkCrosshairLabelTest2.cpp
    )
endif()

include_directories(${CMAKE_SOURCE_DIR}/Libs/Testing
                    ${CMAKE_CURRENT_BINARY_DIR})

if(CTK_USE_QTTESTING)
  list(APPEND TEST_SOURCES
    ctkActionsWidgetEventTranslatorPlayerTest1.cpp
    ctkAddRemoveComboBoxEventTranslatorPlayerTest1.cpp
    ctkAxesWidgetEventTranslatorPlayerTest1.cpp
    ctkButtonGroupEventTranslatorPlayerTest1.cpp
    ctkCheckableComboBoxEventTranslatorPlayerTest1.cpp
    ctkCheckableHeaderViewEventTranslatorPlayerTest1.cpp
    ctkCheckablePushButtonEventTranslatorPlayerTest1.cpp
    ctkCollapsibleButtonEventTranslatorPlayerTest1.cpp
    ctkCollapsibleGroupBoxEventTranslatorPlayerTest1.cpp
    ctkColorDialogEventTranslatorPlayerTest1.cpp
    ctkColorPickerButtonEventTranslatorPlayerTest1.cpp
    ctkComboBoxEventTranslatorPlayerTest1.cpp
    ctkConsoleEventTranslatorPlayerTest1.cpp
    ctkCoordinatesWidgetEventTranslatorPlayerTest1.cpp
#    ctkDateRangeWidgetEventTranslatorPlayerTest1.cpp
    ctkDirectoryButtonEventTranslatorPlayerTest1.cpp
    ctkDoubleRangeSliderEventTranslatorPlayerTest1.cpp
    ctkDoubleSliderEventTranslatorPlayerTest1.cpp
    ctkExpandButtonEventTranslatorPlayerTest1.cpp
    ctkFileDialogEventTranslatorPlayerTest1.cpp
    ctkFontButtonEventTranslatorPlayerTest1.cpp
    ctkMaterialPropertyWidgetEventTranslatorPlayerTest1.cpp
    ctkMatrixWidgetEventTranslatorPlayerTest1.cpp
    ctkMenuButtonEventTranslatorPlayerTest1.cpp
    ctkMenuEventTranslatorPlayerTest1.cpp
    ctkMenuComboBoxEventTranslatorPlayerTest1.cpp
    ctkModalityWidgetEventTranslatorPlayerTest1.cpp
    ctkPathLineEditEventTranslatorPlayerTest1.cpp
    ctkPopupWidgetEventTranslatorPlayerTest1.cpp
    ctkRangeSliderEventTranslatorPlayerTest1.cpp
    ctkRangeWidgetEventTranslatorPlayerTest1.cpp
#    ctkScreenshotDialogEventTranslatorPlayerTest1.cpp
    ctkSearchBoxEventTranslatorPlayerTest1.cpp
    ctkSettingsDialogEventTranslatorPlayerTest1.cpp
    ctkSliderWidgetEventTranslatorPlayerTest1.cpp
    ctkTreeComboBoxEventTranslatorPlayerTest1.cpp
#    ctkWorkflowAbstractPagedWidgetEventTranslatorPlayerTest1.cpp
#    ctkWorkflowButtonBoxWidgetEventTranslatorPlayerTest1.cpp
#    ctkWorkflowGroupBoxEventTranslatorPlayerTest1.cpp
#    ctkWorkflowStackedWidgetEventTranslatorPlayerTest1.cpp
#    ctkWorkflowTabWidgetEventTranslatorPlayerTest1.cpp
#    ctkWorkflowWidgetEventTranslatorPlayerTest1.cpp
#    ctkWorkflowWidgetStepEventTranslatorPlayerTest1.cpp
    )
endif()

create_test_sourcelist(Tests ${KIT}CppTests.cxx

  ${TEST_SOURCES}
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cxx)

set(LIBRARY_NAME ${PROJECT_NAME})

set(Tests_SRCS
  ctkExampleDerivedWorkflowWidgetStep.cpp
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.cpp
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  ctkSettingsPanelTest2Helper.cpp
  ctkSettingsPanelTest2Helper.h
  )

set(Tests_MOC_SRCS
  ctkExampleDerivedWorkflowWidgetStep.h
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h
  ctkSettingsPanelTest2Helper.h
  )

if(CTK_USE_QTTESTING)
  include(${CMAKE_SOURCE_DIR}/Libs/QtTesting/CMake/ctkQtTesting.cmake)
  ctkQtTesting(Tests_SRCS Tests_MOC_SRCS Tests_UI_FORMS Tests_RESOURCES)
endif()

set(Tests_MOC_CPPS
  ctkConsoleTest.cpp
  ctkCoordinatesWidgetTest.cpp
  ctkCoordinatesWidgetValueProxyTest.cpp
  ctkDoubleRangeSliderTest.cpp
  ctkDoubleRangeSliderValueProxyTest.cpp
  ctkDoubleSliderTest.cpp
  ctkDoubleSliderValueProxyTest.cpp
  ctkDoubleSpinBoxTest.cpp
  ctkDoubleSpinBoxValueProxyTest.cpp
  ctkFlatProxyModelTest.cpp
  ctkFontButtonTest.cpp
  ctkLanguageComboBoxTest.cpp
  ctkMatrixWidgetTest.cpp
  ctkMessageBoxDontShowAgainTest.cpp
  ctkPathListWidgetTest.cpp
  ctkPathListWidgetWithButtonsTest.cpp
  ctkPushButtonTest.cpp
  ctkRangeSliderTest.cpp
  ctkRangeWidgetTest.cpp
  ctkRangeWidgetValueProxyTest.cpp
  ctkSearchBoxTest.cpp
  ctkSettingsPanelTest.cpp
  ctkSliderWidgetTest.cpp
  ctkSliderWidgetValueProxyTest.cpp
  )

set(Tests_MOC_CPP)
set(Tests_UI_CPP)
set(Tests_RESOURCES_SRCS)
if(CTK_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_cpp(Tests_MOC_CPP ${Tests_MOC_SRCS})
  qt5_generate_mocs(${Tests_MOC_CPPS})
  if(TEST_UI_FORMS)
    qt5_wrap_ui(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  qt5_add_resources(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
else()
  QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
  QT4_GENERATE_MOCS(${Tests_MOC_CPPS})
  if(TEST_UI_FORMS)
    QT4_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP} ${Tests_UI_CPP} ${Tests_RESOURCES_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::Test)
  if(CTK_USE_QTTESTING)
    list(APPEND KIT_target_libraries Qt5::Xml Qt5::XmlPatterns)
  endif()
  target_link_libraries(${KIT}CppTests ${KIT_target_libraries})
endif()
if(CTK_USE_QTTESTING)
  target_link_libraries(${KIT}CppTests CTKQtTesting)
endif()
#
# Add Tests
#

SIMPLE_TEST( ctkActionsWidgetTest1 )
SIMPLE_TEST( ctkAddRemoveComboBoxTest1 )
SIMPLE_TEST( ctkAxesWidgetTest1 )
SIMPLE_TEST( ctkButtonGroupTest1 )
SIMPLE_TEST( ctkCheckableModelHelperTest1 )
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
SIMPLE_TEST( ctkConsoleTest )
SIMPLE_TEST( ctkConsoleTest1 )
SIMPLE_TEST( ctkCoordinatesWidgetTest )
SIMPLE_TEST( ctkCoordinatesWidgetTest1 )
SIMPLE_TEST( ctkCoordinatesWidgetValueProxyTest )
SIMPLE_TEST( ctkCrosshairLabelTest1 )
SIMPLE_TEST( ctkDateRangeWidgetTest1 )
SIMPLE_TEST( ctkDirectoryButtonTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest )
SIMPLE_TEST( ctkDoubleRangeSliderTest1 )
SIMPLE_TEST( ctkDoubleRangeSliderTest2 )
SIMPLE_TEST( ctkDoubleRangeSliderValueProxyTest )
SIMPLE_TEST( ctkDoubleSliderTest )
SIMPLE_TEST( ctkDoubleSliderTest1 )
SIMPLE_TEST( ctkDoubleSliderValueProxyTest )
SIMPLE_TEST( ctkDoubleSpinBoxTest )
SIMPLE_TEST( ctkDoubleSpinBoxTest1 )
SIMPLE_TEST( ctkDoubleSpinBoxValueProxyTest )
SIMPLE_TEST( ctkDynamicSpacerTest1 )
SIMPLE_TEST( ctkDynamicSpacerTest2 )
SIMPLE_TEST( ctkErrorLogFDMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkErrorLogModelTest1 )
SIMPLE_TEST( ctkErrorLogModelEntryGroupingTest1 )
SIMPLE_TEST( ctkErrorLogModelTerminalOutputTest1 --test-launcher $<TARGET_FILE:${KIT}CppTests>)
SIMPLE_TEST( ctkErrorLogModelTest4 )
SIMPLE_TEST( ctkErrorLogQtMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkErrorLogStreamMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkErrorLogWidgetTest1 )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingDerivedSteps )
SIMPLE_TEST( ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots )
SIMPLE_TEST( ctkExpandableWidgetTest1 )
SIMPLE_TEST( ctkExpandButtonTest1 )
SIMPLE_TEST( ctkFlatProxyModelTest )
SIMPLE_TEST( ctkFileDialogTest1 )
SIMPLE_TEST( ctkFittedTextBrowserTest1 )
SIMPLE_TEST( ctkFlowLayoutTest1 )
SIMPLE_TEST( ctkFontButtonTest )
SIMPLE_TEST( ctkHistogramTest1 )
SIMPLE_TEST( ctkLanguageComboBoxTest )
SIMPLE_TEST( ctkLayoutManagerTest1 )
SIMPLE_TEST( ctkMaterialPropertyPreviewLabelTest1 )
SIMPLE_TEST( ctkMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkMaterialPropertyWidgetTest2 )
SIMPLE_TEST( ctkMatrixWidgetTest )
SIMPLE_TEST( ctkMatrixWidgetTest1 )
SIMPLE_TEST( ctkMatrixWidgetTest2 )
SIMPLE_TEST( ctkMenuButtonTest1 )
SIMPLE_TEST( ctkMenuComboBoxTest1 )
SIMPLE_TEST( ctkMenuComboBoxTest2 )
SIMPLE_TEST( ctkMenuComboBoxTest3 )
SIMPLE_TEST( ctkMenuComboBoxTest4 )
SIMPLE_TEST( ctkMessageBoxDontShowAgainTest )
SIMPLE_TEST( ctkModalityWidgetTest1 )
SIMPLE_TEST( ctkPathLineEditTest1 )
SIMPLE_TEST( ctkPathListWidgetTest )
SIMPLE_TEST( ctkPathListWidgetWithButtonsTest )
SIMPLE_TEST( ctkPopupWidgetTest1 )
SIMPLE_TEST( ctkProxyStyleTest1 )
SIMPLE_TEST( ctkPushButtonTest )
SIMPLE_TEST( ctkRangeSliderTest )
SIMPLE_TEST( ctkRangeSliderTest1 )
SIMPLE_TEST( ctkRangeWidgetTest )
SIMPLE_TEST( ctkRangeWidgetTest1 )
SIMPLE_TEST( ctkRangeWidgetValueProxyTest )
SIMPLE_TEST( ctkScreenshotDialogTest1 )
SIMPLE_TEST( ctkSearchBoxTest )
SIMPLE_TEST( ctkSearchBoxTest1 )
SIMPLE_TEST( ctkSearchBoxTest2 )
SIMPLE_TEST( ctkSettingsDialogTest1 )
SIMPLE_TEST( ctkSettingsPanelTest )
SIMPLE_TEST( ctkSettingsPanelTest1 )
SIMPLE_TEST( ctkSettingsTest1 )
SIMPLE_TEST( ctkSignalMapperTest1 )
SIMPLE_TEST( ctkSliderWidgetTest )
SIMPLE_TEST( ctkSliderWidgetTest1 )
SIMPLE_TEST( ctkSliderWidgetTest2 )
SIMPLE_TEST( ctkSliderWidgetValueProxyTest )
SIMPLE_TEST( ctkThumbnailListWidgetTest1 )
SIMPLE_TEST( ctkThumbnailLabelTest1 )
SIMPLE_TEST( ctkToolTipTrapperTest1 )
SIMPLE_TEST( ctkTransferFunctionTest1 )
SIMPLE_TEST( ctkTransferFunctionRepresentationTest1 )
SIMPLE_TEST( ctkTransferFunctionRepresentationTest2 )
SIMPLE_TEST( ctkTreeComboBoxTest1 )
SIMPLE_TEST( ctkWidgetsTestingUtilitiesTest )
SIMPLE_TEST( ctkWidgetsUtilsTest1 )
SIMPLE_TEST( ctkWidgetsUtilsTestGrabWidget )
SIMPLE_TEST( ctkWorkflowWidgetTest1 )
SIMPLE_TEST( ctkWorkflowWidgetTest2 )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  set(baseline_relative_location "Libs/Widgets")
  SIMPLE_TEST_WITH_DATA( ctkCrosshairLabelTest2 ${baseline_relative_location})
endif()

# Add Tests expecting CTK_USE_QTTESTING to be set
#
if(CTK_USE_QTTESTING)
  SIMPLE_TEST( ctkActionsWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkAddRemoveComboBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkAxesWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkButtonGroupEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCheckableComboBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCheckableHeaderViewEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCheckablePushButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCollapsibleButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCollapsibleGroupBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkColorDialogEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkColorPickerButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkComboBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkConsoleEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkCoordinatesWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkDateRangeWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkDirectoryButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkDoubleRangeSliderEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkDoubleSliderEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkExpandButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkFileDialogEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkFontButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkMaterialPropertyWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkMatrixWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkMenuButtonEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkMenuComboBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkMenuEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkModalityWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkPathLineEditEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkPopupWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkRangeSliderEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkRangeWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkScreenshotDialogEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkSearchBoxEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkSettingsDialogEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkSliderWidgetEventTranslatorPlayerTest1 )
  SIMPLE_TEST( ctkTreeComboBoxEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowAbstractPagedWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowButtonBoxWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowGroupBoxEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowStackedWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowTabWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowWidgetEventTranslatorPlayerTest1 )
#  SIMPLE_TEST( ctkWorkflowWidgetStepEventTranslatorPlayerTest1 )

  set_tests_properties(
    ctkMenuComboBoxEventTranslatorPlayerTest1
    ctkPathLineEditEventTranslatorPlayerTest1
    PROPERTIES RUN_SERIAL TRUE
    )
endif()
