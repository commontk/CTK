

#
# Helper macro allowing to check if the given flags are supported 
# by the underlying build tool
#
# If the flag(s) is/are supported, they will be appended to the string identified by RESULT_VAR
#
# Usage:
#   ctkFunctionCheckCompilerFlags(FLAGS_TO_CHECK VALID_FLAGS_VAR)
#
# Example:
#
#   set(myflags)
#   ctkFunctionCheckCompilerFlags("-fprofile-arcs" myflags)
#   message(1-myflags:${myflags})
#   ctkFunctionCheckCompilerFlags("-fauto-bugfix" myflags)
#   message(2-myflags:${myflags})
#   ctkFunctionCheckCompilerFlags("-Wall" myflags)
#   message(1-myflags:${myflags})
#
#   The output will be:
#    1-myflags: -fprofile-arcs
#    2-myflags: -fprofile-arcs
#    3-myflags: -fprofile-arcs -Wall

FUNCTION(ctkFunctionCheckCompilerFlags CXX_FLAGS_TO_TEST RESULT_VAR)
  
  IF(CXX_FLAGS_TO_TEST STREQUAL "")
    MESSAGE(FATAL_ERROR "CXX_FLAGS_TO_TEST shouldn't be empty")
  ENDIF()
  
  SET(bindir ${CMAKE_BINARY_DIR})
  SET(srcfile ${bindir}/ctkFunctionCheckCompilerFlags.cpp)
  
  FILE(WRITE ${srcfile} "
#include <iostream>
int main(int, char**) { std::cout << \"Rock climbing is awesome\" << std::endl;}
")
  
  SET(is_valid 0)
  TRY_COMPILE(
    is_valid ${bindir} ${srcfile}
    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS:STRING=${CXX_FLAGS_TO_TEST}"
    )
  
  IF(is_valid)
    SET(${RESULT_VAR} "${${RESULT_VAR}} ${CXX_FLAGS_TO_TEST}" PARENT_SCOPE)
  ENDIF()
  
  MESSAGE(STATUS "Compiler Flags [${CXX_FLAGS_TO_TEST}] supported")
ENDFUNCTION()

