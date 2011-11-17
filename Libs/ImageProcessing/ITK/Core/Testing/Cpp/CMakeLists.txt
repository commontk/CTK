set(KIT ${PROJECT_NAME})

#
# Tests
#
set(TEST_SOURCES
  ctkITKErrorLogMessageHandlerWithThreadsTest1.cpp
  ctkITKErrorLogModelTest1.cpp
  )

#
# Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  list(APPEND TEST_SOURCES
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

#set(KIT_HELPER_SRCS
#  ctkVTKObjectTestHelper.cpp
#  )

#QT4_WRAP_CPP(KIT_HELPER_SRCS ctkVTKObjectTestHelper.h)

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

SIMPLE_TEST( ctkITKErrorLogMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkITKErrorLogModelTest1 )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")

endif()

