set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkAbstractPythonManagerTest.cpp
  #EXTRA_INCLUDE TestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/Libs/Testing
                    ${CMAKE_CURRENT_BINARY_DIR})

set(CTK_QT_TEST_LIBRARY )
if (CTK_QT_VERSION VERSION_GREATER "5")
  find_package(Qt6Test REQUIRED)
  set(CTK_QT_TEST_LIBRARY Qt6::Test)
  QT6_GENERATE_MOCS(
    ctkAbstractPythonManagerTest.cpp
    )
elseif (CTK_QT_VERSION VERSION_GREATER "4")
  find_package(Qt${CTK_QT_VERSION}Test REQUIRED)
  set(CTK_QT_TEST_LIBRARY Qt${CTK_QT_VERSION}::Test)
  QT5_GENERATE_MOCS(
    ctkAbstractPythonManagerTest.cpp
    )
else()
  QT4_GENERATE_MOCS(
    ctkAbstractPythonManagerTest.cpp
    )
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES} ${CTK_QT_TEST_LIBRARY})

#
# Add Tests
#

SIMPLE_TEST( ctkAbstractPythonManagerTest )
