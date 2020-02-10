set(KIT ${PROJECT_NAME})

#
# Tests
#
set(TEST_SOURCES
  ctkITKErrorLogMessageHandlerWithThreadsTest1.cpp
  ctkITKErrorLogModelFileLoggingTest1.cpp
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

# The tests depend on stuff in the CTKWidgets library,
# so add it manually here
include_directories(${CMAKE_SOURCE_DIR}/Libs/Widgets)
include_directories(${CMAKE_BINARY_DIR}/Libs/Widgets)

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${KIT_HELPER_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES} CTKWidgets)

#
# Add Tests
#

SIMPLE_TEST( ctkITKErrorLogMessageHandlerWithThreadsTest1 )
SIMPLE_TEST( ctkITKErrorLogModelFileLoggingTest1 )
SIMPLE_TEST( ctkITKErrorLogModelTest1 )

#
# Add Tests expecting CTKData to be set
#
if(EXISTS "${CTKData_DIR}")
  set(baseline_relative_location "Libs/ImageProcessing/ITK/Core")
  #SIMPLE_TEST(Foo ${baseline_relative_location}/${TESTNAME}.png)
endif()
