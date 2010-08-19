SET(KIT ${PROJECT_NAME})

#
# Tests
#
SET(TEST_SOURCES
  ctkTransferFunctionBarsItemTest1.cpp
  ctkTransferFunctionViewTest1.cpp
  ctkTransferFunctionViewTest2.cpp
  ctkTransferFunctionViewTest3.cpp
  ctkTransferFunctionViewTest4.cpp
  ctkTransferFunctionViewTest5.cpp
  ctkVTKChartViewTest1.cpp
  ctkVTKScalarsToColorsViewTest1.cpp
  ctkVTKScalarsToColorsViewTest2.cpp
  ctkVTKScalarsToColorsViewTest3.cpp
  )

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
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)
IF(WIN32)
  SET(KIT_TESTS ${CPP_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CppTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} -D "${CTKData_DIR}/Data")
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkTransferFunctionBarsItemTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest1 )
SIMPLE_TEST( ctkTransferFunctionViewTest2 )
SIMPLE_TEST( ctkTransferFunctionViewTest3 )
SIMPLE_TEST( ctkTransferFunctionViewTest4 )
SIMPLE_TEST( ctkTransferFunctionViewTest5 )
SIMPLE_TEST( ctkVTKChartViewTest1 )
SIMPLE_TEST( ctkVTKScalarsToColorsViewTest1 )
SIMPLE_TEST( ctkVTKScalarsToColorsViewTest2 )
SIMPLE_TEST( ctkVTKScalarsToColorsViewTest3 )

#
# Add Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  #
  SIMPLE_TEST( ctkVTKSliceViewTest1 )
  SIMPLE_TEST( ctkVTKRenderViewTest1 )
ENDIF()
