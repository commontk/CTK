SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ctkDICOMHostTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${KIT_target_libraries} ${QT_LIBRARIES})

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
SIMPLE_TEST(ctkDICOMHostTest1)
