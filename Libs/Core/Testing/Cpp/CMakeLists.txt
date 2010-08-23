SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ctkAbstractFactoryTest1.cpp
  ctkAbstractLibraryFactoryTest1.cpp
  ctkAbstractObjectFactoryTest1.cpp
  ctkAbstractPluginFactoryTest1.cpp
  ctkAbstractQObjectFactoryTest1.cpp
  ctkCommandLineParserTest1.cpp
  ctkModelTesterTest1.cpp
  ctkUtilsTest1.cpp
  ctkDependencyGraphTest1.cpp
  ctkDependencyGraphTest2.cpp
  ctkPimplTest1.cpp
  ctkSingletonTest1.cpp
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

SIMPLE_TEST( ctkCommandLineParserTest1 )
SIMPLE_TEST( ctkDependencyGraphTest1 )
SIMPLE_TEST( ctkDependencyGraphTest2 )
SIMPLE_TEST( ctkModelTesterTest1 )
SIMPLE_TEST( ctkPimplTest1 )
SIMPLE_TEST( ctkSingletonTest1 )
SIMPLE_TEST( ctkUtilsTest1 )
