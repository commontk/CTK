SET(KIT ${PROJECT_NAME})

#
# Tests
#
SET(TEST_SOURCES
  ctkVTKDataSetArrayComboBoxTest1.cpp
  ctkVTKDataSetModelTest1.cpp
  ctkVTKMatrixWidgetTest1.cpp
  ctkTransferFunctionBarsItemTest1.cpp
  ctkTransferFunctionViewTest1.cpp
  ctkTransferFunctionViewTest2.cpp
  ctkTransferFunctionViewTest3.cpp
  ctkTransferFunctionViewTest4.cpp
  ctkTransferFunctionViewTest5.cpp
  ctkVTKSurfaceMaterialPropertyWidgetTest1.cpp
  ctkVTKTextPropertyWidgetTest1.cpp
  ctkVTKThumbnailViewTest1.cpp
  )

IF(CTK_USE_CHARTS)
  SET(TEST_SOURCES
      ctkVTKChartViewTest1.cpp
      ctkVTKScalarsToColorsViewTest1.cpp
      ctkVTKScalarsToColorsViewTest2.cpp
      ctkVTKScalarsToColorsViewTest3.cpp
      ctkVTKScalarsToColorsViewTest4.cpp
      ctkVTKScalarsToColorsWidgetTest1.cpp
      ${TEST_SOURCES})
ENDIF(CTK_USE_CHARTS)

#
# Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  LIST(APPEND TEST_SOURCES
    ctkVTKSliceViewTest1.cpp
    ctkVTKRenderViewTest1.cpp
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
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} -D "${CTKData_DIR}/Data")
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkVTKDataSetArrayComboBoxTest1 )
SIMPLE_TEST( ctkVTKDataSetModelTest1 )
SIMPLE_TEST( ctkVTKMatrixWidgetTest1 )
SIMPLE_TEST( ctkTransferFunctionBarsItemTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest2 )
SIMPLE_TEST( ctkTransferFunctionViewTest3 )
SIMPLE_TEST( ctkTransferFunctionViewTest4 )
SIMPLE_TEST( ctkTransferFunctionViewTest5 )
IF (CTK_USE_CHARTS)
  SIMPLE_TEST( ctkVTKChartViewTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest1 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest2 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest3 )
  SIMPLE_TEST( ctkVTKScalarsToColorsViewTest4 )
  SIMPLE_TEST( ctkVTKScalarsToColorsWidgetTest1 )
ENDIF(CTK_USE_CHARTS)
SIMPLE_TEST( ctkVTKSurfaceMaterialPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKTextPropertyWidgetTest1 )
SIMPLE_TEST( ctkVTKThumbnailViewTest1 )

#
# Add Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  #
  SIMPLE_TEST( ctkVTKSliceViewTest1 )
  SIMPLE_TEST( ctkVTKRenderViewTest1 )
ENDIF()
