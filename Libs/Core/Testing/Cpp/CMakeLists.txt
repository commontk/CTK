# Dummy plugin
ADD_DEFINITIONS( -DCTKDummyPlugin)
ctkMacroBuildLib(
  NAME "CTKDummyPlugin"
  EXPORT_DIRECTIVE "CTK_DUMMY_EXPORT"
  SRCS ctkDummyPlugin.h ctkDummyPlugin.cpp
  MOC_SRCS "ctkDummyPlugin.h"
  TARGET_LIBRARIES ${CTK_BASE_LIBRARIES}
  LIBRARY_TYPE "SHARED"
  DISABLE_WRAP_PYTHONQT
  )
GET_TARGET_PROPERTY(ctkDummyPluginPATH "CTKDummyPlugin" LOCATION)
REMOVE_DEFINITIONS(-DCTKDummyPlugin)

#normal test
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
  ctkWorkflowTest1.cpp
  ctkWorkflowTest2.cpp
  ctkWorkflowTest3.cpp
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

SET(LIBRARY_NAME ${PROJECT_NAME})

SET(Tests_SRCS
  ctkBranchingWorkflowStep.h
  ctkExampleDerivedWorkflowStep.cpp
  ctkExampleDerivedWorkflowStep.h
  ctkExampleWorkflowStepUsingSignalsAndSlots.cpp
  ctkExampleWorkflowStepUsingSignalsAndSlots.h
  ctkSingletonTestHelper.cpp
  ctkSingletonTestHelper.h
  )

SET(Tests_MOC_SRCS
  ctkExampleWorkflowStepUsingSignalsAndSlots.h
  )

SET(Tests_MOC_CPP)
QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})

# Test library
ADD_EXECUTABLE(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES} CTKDummyPlugin)

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkAbstractFactoryTest1 )
SIMPLE_TEST( ctkAbstractLibraryFactoryTest1 )
SIMPLE_TEST( ctkAbstractObjectFactoryTest1 )
SIMPLE_TEST( ctkAbstractPluginFactoryTest1 "totto")
ADD_TEST( ctkAbstractPluginFactoryTest1 ${KIT_TESTS} ctkAbstractPluginFactoryTest1 ${ctkDummyPluginPATH})
SET_PROPERTY(TEST ctkAbstractPluginFactoryTest1 PROPERTY LABELS ${PROJECT_NAME})
SIMPLE_TEST( ctkAbstractQObjectFactoryTest1 )
SIMPLE_TEST( ctkCommandLineParserTest1 )
SIMPLE_TEST( ctkDependencyGraphTest1 )
SIMPLE_TEST( ctkDependencyGraphTest2 )
SIMPLE_TEST( ctkModelTesterTest1 )
SIMPLE_TEST( ctkPimplTest1 )
SIMPLE_TEST( ctkSingletonTest1 )
SIMPLE_TEST( ctkUtilsTest1 )
SIMPLE_TEST( ctkWorkflowTest1 )
SIMPLE_TEST( ctkWorkflowTest2 )
SIMPLE_TEST( ctkWorkflowTest3 )
