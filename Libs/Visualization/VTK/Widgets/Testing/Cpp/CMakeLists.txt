SET(KIT ${PROJECT_NAME})

#
# Tests
#
SET(TEST_SOURCES
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

IF(CTK_USE_CHARTS)
  SET(TEST_SOURCES
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
ENDIF(CTK_USE_CHARTS)

#
# Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  LIST(APPEND TEST_SOURCES
    ctkVTKMagnifyViewTest2.cpp
    ctkVTKSliceViewTest2.cpp
    ctkVTKRenderViewTest2.cpp
    )
ENDIF()

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ${TEST_SOURCES}
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME} vtkCharts ${CTK_BASE_LIBRARIES})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME}
            -D "${CTKData_DIR}/Data"
            -V "${CTKData_DIR}/Baseline/Libs/Visualization/VTK/Widgets"
            )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

MACRO( MAGNIFY_WIDGET_TEST  TESTNAME  TESTTYPE  SIZE  MAGNIFICATION)
  ADD_TEST( ${TESTNAME}${TESTTYPE} ${KIT_TESTS} ${TESTNAME}
            -D "${CTKData_DIR}/Data"
            -V "${CTKData_DIR}/Baseline/Libs/Visualization/VTK/Widgets"
            -T "${TESTTYPE}"
            -S "${SIZE}"
            -M "${MAGNIFICATION}"
            )
  SET_PROPERTY(TEST ${TESTNAME}${TESTTYPE} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( MAGNIFY_WIDGET_TEST  )

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
IF (CTK_USE_CHARTS)
  SIMPLE_TEST( ctkVTKChartViewTest1 )
  SIMPLE_TEST( ctkVTKVolumePropertyWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest3 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest4 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest3 )
ENDIF(CTK_USE_CHARTS)
SIMPLE_TEST( ctkVTKRenderViewTest1 )
SIMPLE_TEST( ctkVTKSliceViewTest1 )
SIMPLE_TEST( ctkVTKSurfaceMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKTextPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKThumbnailViewTest1 )
SIMPLE_TEST( ctkVTKWidgetsUtilsTestGrabWidget )

#
# Add Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  #
  # Dependencies required so that these tests won't run in parallel
  #
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 OddOdd 341 17 )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 EvenEven 340 18 )
  SET_TESTS_PROPERTIES( ctkVTKMagnifyViewTest2EvenEven
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2OddOdd )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 OddEven 341 18 )
  SET_TESTS_PROPERTIES( ctkVTKMagnifyViewTest2OddEven
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2EvenEven )
  MAGNIFY_WIDGET_TEST( ctkVTKMagnifyViewTest2 EvenOdd 340 17 )
  SET_TESTS_PROPERTIES( ctkVTKMagnifyViewTest2EvenOdd
                        PROPERTIES DEPENDS
                        ctkVTKMagnifyViewTest2OddEven )
  SIMPLE_TEST( ctkVTKSliceViewTest2 )
  SIMPLE_TEST( ctkVTKRenderViewTest2 )
ENDIF()

