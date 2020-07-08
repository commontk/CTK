# Dummy plugin used by ctkAbstractPluginFactoryTest1
add_definitions( -DCTKDummyPlugin)

ctkMacroBuildLib(
  NAME "CTKDummyPlugin"
  EXPORT_DIRECTIVE "CTK_DUMMY_EXPORT"
  SRCS ctkDummyPlugin.h ctkDummyPlugin.cpp
  MOC_SRCS "ctkDummyPlugin.h"
  TARGET_LIBRARIES ${CTK_BASE_LIBRARIES}
  LIBRARY_TYPE "SHARED"
  )
remove_definitions(-DCTKDummyPlugin)

set(KIT ${PROJECT_NAME})

#
# Test sources
#
set(KITTests_SRCS
  ctkAbstractFactoryTest1.cpp
  ctkAbstractLibraryFactoryTest1.cpp
  ctkAbstractObjectFactoryTest1.cpp
  ctkAbstractPluginFactoryTest1.cpp
  ctkAbstractQObjectFactoryTest1.cpp
  ctkBackTraceTest.cpp
  ctkBooleanMapperTest.cpp
  ctkCallbackTest1.cpp
  ctkCommandLineParserTest1.cpp
  ctkCoreSettingsTest.cpp
  ctkCoreTestingMacrosTest.cpp
  ctkCoreTestingUtilitiesTest.cpp
  ctkExceptionTest.cpp
  ctkFileLoggerTest.cpp
  ctkHighPrecisionTimerTest.cpp
  ctkLinearValueProxyTest.cpp
  ctkLoggerTest1.cpp
  ctkModelTesterTest1.cpp
  ctkModelTesterTest2.cpp
  ctkUtilsCopyDirRecursivelyTest1.cpp
  ctkUtilsQtHandleToStringTest1.cpp
  ctkUtilsTest.cpp
  ctkUtilsTest1.cpp
  ctkUtilsTest2.cpp
  ctkUtilsTest3.cpp
  ctkUtilsTest4.cpp
  ctkDependencyGraphTest1.cpp
  ctkDependencyGraphTest2.cpp
  ctkPimplTest1.cpp
  ctkScopedCurrentDirTest1.cpp
  ctkSingletonTest1.cpp
  ctkWorkflowTest1.cpp
  ctkWorkflowTest2.cpp
  ctkWorkflowTest3.cpp
  )
if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KITTests_SRCS
    ctkUtf8Test1.cpp
    )
endif()
if(HAVE_BFD)
  list(APPEND KITTests_SRCS
    ctkBinaryFileDescriptorTest1.cpp
    )
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}/Libs/Testing
  ${CMAKE_CURRENT_BINARY_DIR}
  )

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ${KITTests_SRCS}
  #EXTRA_INCLUDE TestingMacros.h
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

#
# Tests Helpers sources
#
set(Tests_Helpers_SRCS
  ctkBranchingWorkflowStep.h
  ctkExampleDerivedWorkflowStep.cpp
  ctkExampleDerivedWorkflowStep.h
  ctkExampleWorkflowStepUsingSignalsAndSlots.cpp
  ctkExampleWorkflowStepUsingSignalsAndSlots.h
  ctkSingletonTestHelper.cpp
  ctkSingletonTestHelper.h
  )

set(Tests_Helpers_MOC_SRCS
  ctkExampleWorkflowStepUsingSignalsAndSlots.h
  )

set(Tests_Helpers_MOC_CPPS
  ctkBooleanMapperTest.cpp
  ctkCoreSettingsTest.cpp
  ctkFileLoggerTest.cpp
  ctkLinearValueProxyTest.cpp
  ctkUtilsTest.cpp
  )

set(Tests_Helpers_MOC_CPP)
if(CTK_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_cpp(Tests_Helpers_MOC_CPP ${Tests_Helpers_MOC_SRCS})
  qt5_generate_mocs(${Tests_Helpers_MOC_CPPS})
else()
  QT4_WRAP_CPP(Tests_Helpers_MOC_CPP ${Tests_Helpers_MOC_SRCS})
  QT4_GENERATE_MOCS(${Tests_Helpers_MOC_CPPS})
endif()

if(HAVE_BFD)
  ctk_add_executable_utf8(ctkBinaryFileDescriptorTestHelper ctkBinaryFileDescriptorTestHelper.cpp)
endif()

if(MSVC)
  add_definitions( /D _CRT_SECURE_NO_WARNINGS)
endif()

#
# Test executable
#
ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_Helpers_SRCS} ${Tests_Helpers_MOC_CPP})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES} CTKDummyPlugin)

if(CTK_QT_VERSION VERSION_GREATER "4")
  find_package(Qt5 COMPONENTS Test Widgets REQUIRED)
  target_link_libraries(${KIT}CppTests Qt5::Test Qt5::Widgets)
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(${KIT}CppTests rt)
endif()

#
# Add Tests
#

SIMPLE_TEST( ctkAbstractFactoryTest1 )
SIMPLE_TEST( ctkAbstractLibraryFactoryTest1 $<TARGET_FILE:CTKDummyPlugin> )
SIMPLE_TEST( ctkAbstractObjectFactoryTest1 )
SIMPLE_TEST( ctkAbstractPluginFactoryTest1 $<TARGET_FILE:CTKDummyPlugin> )
SIMPLE_TEST( ctkAbstractQObjectFactoryTest1 )
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  SIMPLE_TEST( ctkBackTraceTest )
endif()
if(HAVE_BFD)
  SIMPLE_TEST( ctkBinaryFileDescriptorTest1 $<TARGET_FILE:ctkBinaryFileDescriptorTestHelper> )
endif()
SIMPLE_TEST( ctkBooleanMapperTest )
SIMPLE_TEST( ctkCallbackTest1 )
SIMPLE_TEST( ctkCommandLineParserTest1 )
SIMPLE_TEST( ctkCoreSettingsTest )
SIMPLE_TEST( ctkCoreTestingMacrosTest )
SIMPLE_TEST( ctkCoreTestingUtilitiesTest )
SIMPLE_TEST( ctkDependencyGraphTest1 )
SIMPLE_TEST( ctkDependencyGraphTest2 )
SIMPLE_TEST( ctkExceptionTest )
SIMPLE_TEST( ctkFileLoggerTest )
SIMPLE_TEST( ctkHighPrecisionTimerTest )
SIMPLE_TEST( ctkLinearValueProxyTest )
SIMPLE_TEST( ctkLoggerTest1 )
set_property(TEST ctkLoggerTest1 PROPERTY PASS_REGULAR_EXPRESSION "logger.debug\nlogger.info\nlogger.trace\nlogger.warn\nlogger.error\nlogger.fatal")
SIMPLE_TEST( ctkModelTesterTest1 )
SIMPLE_TEST( ctkModelTesterTest2 )
SIMPLE_TEST( ctkPimplTest1 )
SIMPLE_TEST( ctkScopedCurrentDirTest1 )
SIMPLE_TEST( ctkSingletonTest1 )
SIMPLE_TEST( ctkUtilsCopyDirRecursivelyTest1 )
SIMPLE_TEST( ctkUtilsQtHandleToStringTest1 )
SIMPLE_TEST( ctkUtilsTest )
SIMPLE_TEST( ctkUtilsTest1 )
SIMPLE_TEST( ctkUtilsTest2 )
SIMPLE_TEST( ctkUtilsTest3 )
SIMPLE_TEST( ctkUtilsTest4 )
SIMPLE_TEST( ctkWorkflowTest1 )
SIMPLE_TEST( ctkWorkflowTest2 )
SIMPLE_TEST( ctkWorkflowTest3 )
