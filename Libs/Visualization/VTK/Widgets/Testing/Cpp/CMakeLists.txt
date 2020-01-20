set(KIT ${PROJECT_NAME})

#
# Tests
#
set(TEST_SOURCES
  ctkVTKAbstractViewTest1.cpp
  ctkVTKColorTransferFunctionTest1.cpp
  ctkVTKDataSetArrayComboBoxTest1.cpp
  ctkVTKDataSetModelTest1.cpp
  ctkVTKErrorLogMessageHandlerWithThreadsTest1.cpp
  ctkVTKErrorLogModelFileLoggingTest1.cpp
  ctkVTKErrorLogModelTest1.cpp
  ctkVTKHistogramTest1.cpp
  ctkVTKHistogramTest2.cpp
  ctkVTKHistogramTest3.cpp
  ctkVTKHistogramTest4.cpp
  ctkVTKMatrixWidgetTest1.cpp
  ctkVTKMagnifyViewTest1.cpp
  ctkVTKScalarBarWidgetTest1.cpp
  ctkVTKThresholdWidgetTest1.cpp
  ctkTransferFunctionBarsItemTest1.cpp
  ctkTransferFunctionViewTest1.cpp
  ctkTransferFunctionViewTest2.cpp
  ctkTransferFunctionViewTest3.cpp
  ctkTransferFunctionViewTest4.cpp
  ctkTransferFunctionViewTest5.cpp
  ctkVTKPropertyWidgetTest.cpp
  ctkVTKRenderViewTest1.cpp
  ctkVTKScalarsToColorsComboBoxTest1.cpp
  ctkVTKScalarsToColorsUtilsTest1.cpp
  ctkVTKSliceViewTest1.cpp
  ctkVTKSurfaceMaterialPropertyWidgetTest1.cpp
  ctkVTKTextPropertyWidgetTest1.cpp
  ctkVTKThumbnailViewTest1.cpp
  ctkVTKTransferFunctionRepresentationTest1.cpp
  ctkVTKWidgetsUtilsTestGrabWidget.cpp
  ctkVTKWidgetsUtilsTestImageConversion.cpp
  )

if(CTK_USE_CHARTS)
  set(TEST_SOURCES
      ctkVTKChartViewTest1.cpp
      ctkVTKVolumePropertyWidgetTest1.cpp
      ctkVTKDiscretizableColorTransferWidgetTest1.cpp
      ctkVTKScalarsToColorsViewTest1.cpp
      ctkVTKScalarsToColorsViewTest2.cpp
      ctkVTKScalarsToColorsViewTest3.cpp
      ctkVTKScalarsToColorsViewTest4.cpp
      ctkVTKScalarsToColorsWidgetTest1.cpp
      ctkVTKScalarsToColorsWidgetTest2.cpp
      ctkVTKScalarsToColorsWidgetTest3.cpp
      ${TEST_SOURCES})
endif()

#
# Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  list(APPEND TEST_SOURCES
    ctkVTKMagnifyViewTest2.cpp
    ctkVTKSliceViewTest2.cpp
    ctkVTKRenderViewTest2.cpp
    )
endif()

#
# Test expecting CTK_USE_QTTESTING to be ON
#
if(CTK_USE_QTTESTING)
  list(APPEND TEST_SOURCES
    ctkVTKDataSetArrayComboBoxEventTranslatorPlayerTest1.cpp
    ctkVTKDataSetModelEventTranslatorPlayerTest1.cpp
#    ctkVTKMagnifyViewEventTranslatorPlayerTest1.cpp
    ctkVTKMatrixWidgetEventTranslatorPlayerTest1.cpp
    ctkVTKRenderViewEventTranslatorPlayerTest1.cpp
    ctkVTKScalarBarWidgetEventTranslatorPlayerTest1.cpp
#    ctkVTKScalarsToColorsUtilsEventTranslatorPlayerTest1.cpp
#    ctkVTKScalarsToColorsViewEventTranslatorPlayerTest1.cpp
#    ctkVTKScalarsToColorsWidgetEventTranslatorPlayerTest1.cpp
#    ctkVTKSliceViewEventTranslatorPlayerTest1.cpp
#    ctkVTKSurfaceMaterialPropertyWidgetEventTranslatorPlayerTest1.cpp
    ctkVTKTextPropertyWidgetEventTranslatorPlayerTest1.cpp
#    ctkVTKThresholdWidgetEventTranslatorPlayerTest1.cpp
#    ctkVTKVolumePropertyWidgetEventTranslatorPlayerTest1.cpp
    )
endif()

include_directories(${CMAKE_SOURCE_DIR}/Libs/Testing
                    ${CMAKE_CURRENT_BINARY_DIR})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ${TEST_SOURCES}
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

if(CTK_USE_QTTESTING)
  include(${CMAKE_SOURCE_DIR}/Libs/QtTesting/CMake/ctkQtTesting.cmake)
  ctkQtTesting(Tests TEST_MOC_SOURCES TEST_UI_FORMS Tests_RESOURCES)
endif()

set(TEST_MOC_CPP)
set(TEST_UI_CPP)
set(Tests_RESOURCES_SRCS)
if(CTK_QT_VERSION VERSION_GREATER "4")
  if(TEST_MOC_SOURCES)
    QT5_WRAP_CPP(TEST_MOC_CPP ${TEST_MOC_SOURCES})
  endif()
  QT5_GENERATE_MOCS(
    ctkVTKPropertyWidgetTest.cpp
    )
  if(TEST_UI_FORMS)
    QT5_WRAP_UI(TEST_UI_CPP ${TEST_UI_FORMS})
  endif()
  QT5_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
else()
  if(TEST_MOC_SOURCES)
    QT4_WRAP_CPP(TEST_MOC_CPP ${TEST_MOC_SOURCES})
  endif()
  QT4_GENERATE_MOCS(
    ctkVTKPropertyWidgetTest.cpp
    )
  if(TEST_UI_FORMS)
    QT4_WRAP_UI(TEST_UI_CPP ${TEST_UI_FORMS})
  endif()
  QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${TEST_MOC_CPP} ${TEST_UI_CPP} ${Tests_RESOURCES_SRCS})
if(${VTK_VERSION} VERSION_EQUAL 8.90 OR ${VTK_VERSION} VERSION_GREATER 8.90 )
  set(VTK_CHARTS_LIB)
elseif(${VTK_VERSION_MAJOR} GREATER 5)
  set(VTK_CHARTS_LIB vtkChartsCore)
else()
  set(VTK_CHARTS_LIB vtkCharts)
endif()



target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${VTK_CHARTS_LIB} ${CTK_BASE_LIBRARIES})

if(CTK_QT_VERSION VERSION_GREATER "4")
  target_link_libraries(${KIT}CppTests ${Qt5Test_LIBRARIES})
endif()
if(CTK_USE_QTTESTING)
  target_link_libraries(${KIT}CppTests CTKQtTesting)
endif()

#
# Add Tests
#

SIMPLE_TEST( ctkVTKAbstractViewTest1 )
SIMPLE_TEST( ctkVTKColorTransferFunctionTest1 )
SIMPLE_TEST( ctkVTKDataSetArrayComboBoxTest1 )
SIMPLE_TEST( ctkVTKDataSetModelTest1 )
SIMPLE_TEST( ctkVTKErrorLogMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkVTKErrorLogModelFileLoggingTest1 )
SIMPLE_TEST( ctkVTKErrorLogModelTest1 )
SIMPLE_TEST( ctkVTKHistogramTest1 )
SIMPLE_TEST( ctkVTKHistogramTest2 )
SIMPLE_TEST( ctkVTKHistogramTest3 )
SIMPLE_TEST( ctkVTKHistogramTest4 )
SIMPLE_TEST( ctkVTKMagnifyViewTest1 )
SIMPLE_TEST( ctkVTKMatrixWidgetTest1 )
SIMPLE_TEST( ctkVTKPropertyWidgetTest )
SIMPLE_TEST( ctkVTKScalarBarWidgetTest1 )
SIMPLE_TEST( ctkVTKScalarsToColorsUtilsTest1 )
SIMPLE_TEST( ctkVTKThresholdWidgetTest1 )
SIMPLE_TEST( ctkTransferFunctionBarsItemTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest2 )
SIMPLE_TEST( ctkTransferFunctionViewTest3 )
SIMPLE_TEST( ctkTransferFunctionViewTest4 )
SIMPLE_TEST( ctkTransferFunctionViewTest5 )
if(CTK_USE_CHARTS)
  SIMPLE_TEST( ctkVTKChartViewTest1 )
  SIMPLE_TEST( ctkVTKVolumePropertyWidgetTest1 )
  SIMPLE_TEST( ctkVTKDiscretizableColorTransferWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest3 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest4 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest3 )
endif()
SIMPLE_TEST( ctkVTKRenderViewTest1 )
SIMPLE_TEST( ctkVTKScalarsToColorsComboBoxTest1 )
SIMPLE_TEST( ctkVTKSliceViewTest1 )
SIMPLE_TEST( ctkVTKSurfaceMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKTextPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKThumbnailViewTest1 )
SIMPLE_TEST( ctkVTKTransferFunctionRepresentationTest1 )
SIMPLE_TEST( ctkVTKWidgetsUtilsTestGrabWidget )
SIMPLE_TEST( ctkVTKWidgetsUtilsTestImageConversion )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  set(baseline_relative_location Libs/Visualization/VTK/Widgets)

  macro(MAGNIFY_WIDGET_TEST TESTNAME TESTTYPE SIZE MAGNIFICATION)
    set(${TESTNAME}${TESTTYPE}_TEST ${TESTNAME})
    SIMPLE_TEST_WITH_DATA(${TESTNAME}${TESTTYPE} ${baseline_relative_location}
      -T "${TESTTYPE}"
      -S "${SIZE}"
      -M "${MAGNIFICATION}"
      )
  endmacro()

  #
  # Dependencies required so that these tests won't run in parallel
  #
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 OddOdd 341 17 )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 EvenEven 340 18 )
  set_tests_properties( ctkVTKMagnifyViewTest2EvenEven
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2OddOdd )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 OddEven 341 18 )
  set_tests_properties( ctkVTKMagnifyViewTest2OddEven
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2EvenEven )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 EvenOdd 340 17 )
  set_tests_properties( ctkVTKMagnifyViewTest2EvenOdd
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2OddEven )

  SIMPLE_TEST_WITH_DATA( ctkVTKRenderViewTest2 ${baseline_relative_location})
  SIMPLE_TEST_WITH_DATA( ctkVTKSliceViewTest2 ${baseline_relative_location})
endif()

#
# Add Tests expecting CTK_USE_QTTESTING to be set
#
if(CTK_USE_QTTESTING)
    SIMPLE_TEST( ctkVTKDataSetArrayComboBoxEventTranslatorPlayerTest1 )
    SIMPLE_TEST( ctkVTKDataSetModelEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKMagnifyViewEventTranslatorPlayerTest1 )
    SIMPLE_TEST( ctkVTKMatrixWidgetEventTranslatorPlayerTest1 )
    SIMPLE_TEST( ctkVTKRenderViewEventTranslatorPlayerTest1 )
    SIMPLE_TEST( ctkVTKScalarBarWidgetEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKScalarsToColorsUtilsEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKScalarsToColorsViewEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKScalarsToColorsWidgetEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKSliceViewEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKSurfaceMaterialPropertyWidgetEventTranslatorPlayerTest1 )
    SIMPLE_TEST( ctkVTKTextPropertyWidgetEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKThresholdWidgetEventTranslatorPlayerTest1 )
#    SIMPLE_TEST( ctkVTKVolumePropertyWidgetEventTranslatorPlayerTest1 )
endif()
