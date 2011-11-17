set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMIndexerTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${KIT_target_libraries})

set( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)
if(WIN32)
  set(KIT_TESTS ${CPP_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CppTests)
endif()

macro( SIMPLE_TEST  TESTNAME )
  add_test( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  set_property(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
endmacro( SIMPLE_TEST  )

#
# Add Tests
#
SIMPLE_TEST(ctkDICOMIndexerTest1)
