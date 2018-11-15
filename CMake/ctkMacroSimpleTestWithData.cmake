


#! Usage:
#! \code
#! SIMPLE_TEST_WITH_DATA(<testname> <baseline_relative_location> [argument1 ...])
#! \endcode
#!
#! This macro add a test using the complete add_test signature specifying target using 
#! $<TARGET_FILE:...> generator expression. Optionnal test argument(s) can be passed
#! after specifying the <testname>.
#!
#! <baseline_relative_location> parameter should corresponds to a subfolder located in <CTKData_DIR>/Baseline
#!
#! Variables named KIT and CTKData_DIR are expected to be defined in the current scope.
#!
#! KIT variable usually matches the value of PROJECT_NAME.
#!
#! The macro also associates a label to the test based on the current value of KIT.
#!
#! By default, the name of the test to execute is expected to match <testname>, setting
#! variable <testname>_TEST allows to change that.
#!
#! The following parameter will be passed to the test:
#! <ul>
#!   <li>-D <CTKData_DIR>/Data</li>
#!   <li>-V <CTKData_DIR>/Baseline/<baseline_relative_location></li>
#!   <li>-T <PROJECT_BINARY_DIR>/Testing/Temporary</li>
#! </ul>
#!
#! \sa http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:add_test
#! \sa http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:PROJECT_NAME
#!
#! \ingroup CMakeUtilities
macro(SIMPLE_TEST_WITH_DATA testname baseline_relative_location)
  if("${KIT}" STREQUAL "")
    message(FATAL_ERROR "error: KIT variable is not set !")
  endif()
  
  if(NOT TARGET ${KIT}CppTests)
    message(FATAL_ERROR "error: ${KIT}CppTests target does NOT exist !")
  endif()
  
  if(NOT EXISTS "${CTKData_DIR}/Data")
    message(FATAL_ERROR "error: <CTKData_DIR>/Data corresponds to an non-existing directory. [<CTKData_DIR>/Data: ${CTKData_DIR}/Data]")
  endif()
  
  if(NOT EXISTS "${CTKData_DIR}/Baseline/${baseline_relative_location}")
    message(FATAL_ERROR "error: <CTKData_DIR>/Baseline/<baseline_relative_location> corresponds to an non-existing file or directory. [<CTKData_DIR>/Baseline/<baseline_relative_location>: ${CTKData_DIR}/Baseline/${baseline_relative_location}]")
  endif()

  if(NOT DEFINED ${testname}_TEST)
    set(${testname}_TEST ${testname})
  endif()
  
  add_test(NAME ${testname} COMMAND $<TARGET_FILE:${KIT}CppTests> ${${testname}_TEST}
            -D "${CTKData_DIR}/Data"
            -V "${CTKData_DIR}/Baseline/${baseline_relative_location}"
            -T "${PROJECT_BINARY_DIR}/Testing/Temporary"
            ${ARGN}
            )
  set_property(TEST ${testname} PROPERTY LABELS ${KIT})
endmacro()

