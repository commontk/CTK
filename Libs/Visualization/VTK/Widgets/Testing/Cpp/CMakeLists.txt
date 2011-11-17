set(KIT ${PROJECT_NAME})

#
# Tests
#
set(TEST_SOURCES
  ctkVTKDataSetArrayComboBoxTest1.cpp
  ctkVTKDataSetModelTest1.cpp
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
  ctkVTKRenderViewTest1.cpp
  ctkVTKScalarsToColorsUtilsTest1.cpp
  ctkVTKSliceViewTest1.cpp
  ctkVTKSurfaceMaterialPropertyWidgetTest1.cpp
  ctkVTKTextPropertyWidgetTest1.cpp
  ctkVTKThumbnailViewTest1.cpp
  ctkVTKWidgetsUtilsTestGrabWidget.cpp
  )

if(CTK_USE_CHARTS)
  set(TEST_SOURCES
      ctkVTKChartViewTest1.cpp
      ctkVTKVolumePropertyWidgetTest1.cpp
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

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ${TEST_SOURCES}
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} vtkCharts ${CTK_BASE_LIBRARIES})

#
# Add Tests
#

SIMPLE_TEST( ctkVTKDataSetArrayComboBoxTest1 )
SIMPLE_TEST( ctkVTKDataSetModelTest1 )
SIMPLE_TEST( ctkVTKMagnifyViewTest1 )
SIMPLE_TEST( ctkVTKMatrixWidgetTest1 )
SIMPLE_TEST( ctkVTKScalarBarWidgetTest1 )
SIMPLE_TEST( ctkVTKScalarsToColorsUtilsTest1 )
SIMPLE_TEST( ctkVTKThresholdWidgetTest1 )
SIMPLE_TEST( ctkTransferFunctionBarsItemTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest2 )
SIMPLE_TEST( ctkTransferFunctionViewTest3 )
SIMPLE_TEST( ctkTransferFunctionViewTest4 )
SIMPLE_TEST( ctkTransferFunctionViewTest5 )
IF(CTK_USE_CHARTS)
  SIMPLE_TEST( ctkVTKChartViewTest1 )
  SIMPLE_TEST( ctkVTKVolumePropertyWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest3 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest4 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest3 )
endif()
SIMPLE_TEST( ctkVTKRenderViewTest1 )
SIMPLE_TEST( ctkVTKSliceViewTest1 )
SIMPLE_TEST( ctkVTKSurfaceMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKTextPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKThumbnailViewTest1 )
SIMPLE_TEST( ctkVTKWidgetsUtilsTestGrabWidget )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  set(baseline_relative_location Libs/Visualization/VTK/Widgets)

  macro(MAGNIFY_WIDGET_TEST TESTNAME TESTTYPE SIZE MAGNIFICATION)
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

