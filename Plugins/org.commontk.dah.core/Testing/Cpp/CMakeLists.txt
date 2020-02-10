set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cxx
  ctkDicomAppHostingTypesTest1.cpp
  ctkDicomObjectLocatorCacheTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cxx)

set(LIBRARY_NAME ${PROJECT_NAME})

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

#
# Add Tests
#

SIMPLE_TEST( ctkDicomAppHostingTypesTest1 )
SIMPLE_TEST( ctkDicomObjectLocatorCacheTest1 )
