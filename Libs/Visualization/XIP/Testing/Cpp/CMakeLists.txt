set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

#
# Add Tests
#

