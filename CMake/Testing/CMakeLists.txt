#
# Helper macro
#
macro(add_cmakescript_test testname script)
  add_test(cmake_${testname} ${CMAKE_COMMAND}
    -DTEST_${testname}:BOOL=ON
    -P ${CMAKE_SOURCE_DIR}/CMake/${script}.cmake)
  set_tests_properties(cmake_${testname} PROPERTIES
    LABELS CMake
    PASS_REGULAR_EXPRESSION "SUCCESS")
endmacro()

set(CMAKE_TEST_GENERATOR "${CMAKE_GENERATOR}")
set(CMAKE_TEST_GENERATOR_TOOLSET "${CMAKE_GENERATOR_TOOLSET}")
set(CMAKE_TEST_MAKEPROGRAM "${CMAKE_MAKE_PROGRAM}")

set(build_generator_args
  --build-generator ${CMAKE_TEST_GENERATOR}
  --build-makeprogram ${CMAKE_TEST_MAKEPROGRAM}
  )
if(CMAKE_TEST_GENERATOR_TOOLSET)
  list(APPEND build_generator_args
    --build-generator-toolset ${CMAKE_TEST_GENERATOR_TOOLSET}
    )
endif()

#
# Add 'CMake script' Tests
#
add_cmakescript_test(ctk_list_to_string_test ctkListToString)
add_cmakescript_test(_sb_cmakevar_to_cmakearg_test ctkMacroCheckExternalProjectDependencyTest)
add_cmakescript_test(_sb_extract_varname_and_vartype_test ctkMacroCheckExternalProjectDependencyTest)

add_test(cmake_superbuild_test ${CMAKE_CTEST_COMMAND}
    --build-and-test
    "${CMAKE_SOURCE_DIR}/CMake/Testing/SuperBuildTest"
    "${CMAKE_BINARY_DIR}/CMake/Testing/SuperBuildTest"
    ${build_generator_args}
    --build-project SuperBuildTest
    --build-exe-dir "${CMAKE_BINARY_DIR}/CMake/Testing/SuperBuildTest"
    --force-new-ctest-process
    --test-command ${CMAKE_CTEST_COMMAND} -V
    )
set_tests_properties(cmake_superbuild_test PROPERTIES LABELS CMake)
