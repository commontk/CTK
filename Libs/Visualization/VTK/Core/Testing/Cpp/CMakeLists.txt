set(KIT ${PROJECT_NAME})

#
# Tests
#
set(TEST_SOURCES
  ctkVTKColorTransferFunctionTest1.cpp
  ctkVTKConnectionTest1.cpp
  ctkVTKErrorLogMessageHandlerWithThreadsTest1.cpp
  ctkVTKErrorLogModelTest1.cpp
  ctkVTKHistogramTest1.cpp
  ctkVTKHistogramTest2.cpp
  ctkVTKHistogramTest3.cpp
  ctkVTKHistogramTest4.cpp
  ctkVTKObjectTest1.cpp
  ctkVTKTransferFunctionRepresentationTest1.cpp
  )

#
# Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  list(APPEND TEST_SOURCES
    vtkLightBoxRendererManagerTest1.cpp
    )
endif()

#
# Pre-requisites
#

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 )

#
# Test helpers
#

set(KIT_HELPER_SRCS
  ctkVTKObjectTestHelper.cpp
  )

QT4_WRAP_CPP(KIT_HELPER_SRCS ctkVTKObjectTestHelper.h)

#
# Tests
#

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ${TEST_SOURCES}
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests} ${KIT_HELPER_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

set( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

macro( SIMPLE_TEST  TESTNAME )
  add_test( ${TESTNAME} ${KIT_TESTS} ${TESTNAME}
            -D ${CTKData_DIR}/Data
            -V ${CTKData_DIR}/Baseline/Libs/Visualization/VTK/Core/${TESTNAME}.png
            -T ${CTK_BINARY_DIR}/Testing/Temporary
            )
  set_property(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
endmacro( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkVTKColorTransferFunctionTest1 )
SIMPLE_TEST( ctkVTKConnectionTest1 )
SIMPLE_TEST( ctkVTKErrorLogMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkVTKErrorLogModelTest1 )
SIMPLE_TEST( ctkVTKHistogramTest1 )
SIMPLE_TEST( ctkVTKHistogramTest2 )
SIMPLE_TEST( ctkVTKHistogramTest3 )
SIMPLE_TEST( ctkVTKHistogramTest4 )
SIMPLE_TEST( ctkVTKObjectTest1 )
SIMPLE_TEST( ctkVTKTransferFunctionRepresentationTest1 )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  SIMPLE_TEST( vtkLightBoxRendererManagerTest1 )
endif()

