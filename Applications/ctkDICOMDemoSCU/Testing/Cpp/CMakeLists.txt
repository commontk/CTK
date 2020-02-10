set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMDemoSCUTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file '<KIT_SOURCE_DIR>/target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries ${${KIT}_SOURCE_DIR})

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${KIT_target_libraries})

#
# Add Tests
#
SIMPLE_TEST(ctkDICOMDemoSCUTest1 $<TARGET_FILE:ctkDICOMDemoSCU> )
