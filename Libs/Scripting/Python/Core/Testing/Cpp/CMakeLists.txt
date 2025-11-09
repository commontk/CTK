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

find_package(Qt${CTK_QT_VERSION} COMPONENTS Test REQUIRED)
set(CTK_QT_TEST_LIBRARY Qt${CTK_QT_VERSION}::Test)

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES} ${CTK_QT_TEST_LIBRARY})

# Configure CMake Qt automatic code generation
set_target_properties(${KIT}CppTests PROPERTIES
  AUTOMOC ON
  )

#
# Add Tests
#

SIMPLE_TEST( ctkAbstractPythonManagerTest )
