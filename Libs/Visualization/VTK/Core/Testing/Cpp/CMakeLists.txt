SET(KIT ${PROJECT_NAME})

#
# Pre-requisites
#

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 )
  
# Create ctkVTKObjectTestHelper source list

SET(KIT_HELPER_SRCS
  ctkVTKObjectTestHelper.cpp
  )

QT4_WRAP_CPP(KIT_HELPER_SRCS ctkVTKObjectTestHelper.h)

#
# Testing data
#

SET(TESTING_DATA ${CMAKE_CURRENT_SOURCE_DIR}/Data)

# Resource file used for testing is generated running:
#    bin/ctkVisualizationVTKCoreCppTests ctkVTKCommandOptionsTest1 --generate-test-data

SET(CTKVTKCOMMANDOPTIONSTEST_SETTINGS ${TESTING_DATA}/Settings/ctkVTKCommandOptionsTest1.ini)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/ctkVisualizationVTKCoreCppTestsConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkVisualizationVTKCoreCppTestsConfigure.h
  )

#
# Tests
#

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ctkVTKCommandOptionsTest1.cpp
  ctkVTKObjectTest1.cpp
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CppTests ${Tests} ${KIT_HELPER_SRCS})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

SET( KIT_TESTS ${CXX_TEST_PATH}/${KIT}CppTests)
IF(WIN32)
  SET(KIT_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CppTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkVTKObjectTest1 )

ADD_TEST( ctkVTKCommandOptionsTest1 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --help )
          
ADD_TEST( ctkVTKCommandOptionsTest2 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --enable-awesome-test )

ADD_TEST( ctkVTKCommandOptionsTest3 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 -eat )

ADD_TEST( ctkVTKCommandOptionsTest4 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --disable-settings --test-if-disablesettings-flag-works )

SET(arg1 "Testing with")
SET(arg2 " cmake is ")
SET(arg3 "super awesome !")
SET(arg4 "CMake rocks !")

IF(WIN32)
ADD_TEST( ctkVTKCommandOptionsTest5 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --my-list-of-strings "${arg1}" "${arg2}" "${arg3}"
          -ms="${arg4}" --test-if-commandline-overwrite-settings )

ADD_TEST( ctkVTKCommandOptionsTest6 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 -mlos "${arg1}" "${arg2}" "${arg3}"
          --my-string="${arg4}" --test-if-commandline-overwrite-settings )
ELSE()
ADD_TEST( ctkVTKCommandOptionsTest5 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --my-list-of-strings ${arg1} ${arg2} ${arg3}
          -ms=${arg4} --test-if-commandline-overwrite-settings )

ADD_TEST( ctkVTKCommandOptionsTest6 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 -mlos ${arg1} ${arg2} ${arg3}
          --my-string=${arg4} --test-if-commandline-overwrite-settings )
ENDIF()

ADD_TEST( ctkVTKCommandOptionsTest7 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --test-if-ignorerest-flag-works
          --ignore-rest --nothing=foo extra1 /? )

ADD_TEST( ctkVTKCommandOptionsTest8 ${KIT_TESTS}
          ctkVTKCommandOptionsTest1 --test-if-ignorerest-flag-works
          -- --nothing=foo extra1 /? )
