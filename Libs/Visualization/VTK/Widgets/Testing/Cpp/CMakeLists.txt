SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ctkTransferFunctionWidgetTest1.cpp
  ctkTransferFunctionWidgetTest2.cpp
  ctkTransferFunctionWidgetTest3.cpp
  ctkTransferFunctionWidgetTest4.cpp
  ctkTransferFunctionWidgetTest5.cpp
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
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

#SIMPLE_TEST( ctkVTKObjectTest1 )
SIMPLE_TEST( ctkTransferFunctionWidgetTest1 )
SIMPLE_TEST( ctkTransferFunctionWidgetTest2 )
SIMPLE_TEST( ctkTransferFunctionWidgetTest3 )
SIMPLE_TEST( ctkTransferFunctionWidgetTest4 )
SIMPLE_TEST( ctkTransferFunctionWidgetTest5 )
