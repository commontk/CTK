SET(KIT ${PROJECT_NAME})

#
# Tests
#
SET(TEST_SOURCES
  ctkVTKConnectionTest1.cpp
  ctkVTKObjectTest1.cpp
  )

#
# Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  LIST(APPEND TEST_SOURCES
    vtkLightBoxRendererManagerTest1.cpp
    )
ENDIF()

#
# Pre-requisites
#

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 )

#
# Test helpers
#

SET(KIT_HELPER_SRCS
  ctkVTKObjectTestHelper.cpp
  )

QT4_WRAP_CPP(KIT_HELPER_SRCS ctkVTKObjectTestHelper.h)

#
# Tests
#

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ${TEST_SOURCES}
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CppTests ${Tests} ${KIT_HELPER_SRCS})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME}
            -D ${CTKData_DIR}/Data
            -V ${CTKData_DIR}/Baseline/Libs/Visualization/VTK/Core/${TESTNAME}.png
            -T ${CTK_BINARY_DIR}/Testing/Temporary
            )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkVTKObjectTest1 )
SIMPLE_TEST( ctkVTKConnectionTest1 )

#
# Add Tests expecting CTKData to be set
#
IF(EXISTS "${CTKData_DIR}")
  SIMPLE_TEST( vtkLightBoxRendererManagerTest1 )
ENDIF()

