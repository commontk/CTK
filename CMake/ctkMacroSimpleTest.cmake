


#! Usage:
#! \code
#! SIMPLE_TEST(<testname> [argument1 ...])
#! \endcode
#!
#! This macro add a test using the complete add_test signature specifying target using 
#! $<TARGET_FILE:...> generator expression. Optionnal test argument(s) can be passed
#! after specifying the <testname>.
#!
#! Variable named KIT is expected to be defined in the current scope. KIT variable usually 
#! matches the value of PROJECT_NAME.
#!
#! The macro also associates a label to the test based on the current value of KIT.
#!
#! \sa http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:add_test
#! \sa http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:PROJECT_NAME
#!
#! \ingroup CMakeUtilities
macro(SIMPLE_TEST testname)
  if("${KIT}" STREQUAL "")
    message(FATAL_ERROR "error: KIT variable is not set !")
  endif()
  
  if(NOT TARGET ${KIT}CppTests)
    message(FATAL_ERROR "error: ${KIT}CppTests target does NOT exist !")
  endif()
  
  add_test(NAME ${testname} COMMAND $<TARGET_FILE:${KIT}CppTests> ${testname} ${ARGN})
  set_property(TEST ${testname} PROPERTY LABELS ${KIT})
endmacro()

