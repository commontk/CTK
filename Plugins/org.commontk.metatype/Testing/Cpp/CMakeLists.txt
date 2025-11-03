set(SRCS
  ctkMetaTypeImplTestMain.cpp
)

set(test_executable ${PROJECT_NAME}CppTests)

set(${test_executable}_DEPENDENCIES ${fw_lib} ${fwtestutil_lib})

# Ensure "ctkCompatibility_p.h" included from the export header is found
list(APPEND ${test_executable}_DEPENDENCIES CTKCore)

set(my_includes)
ctkFunctionGetIncludeDirs(my_includes ${test_executable})
include_directories(${my_includes})

ctk_add_executable_utf8(${test_executable} ${SRCS})
target_link_libraries(${test_executable}
  ${${test_executable}_DEPENDENCIES}
)

add_dependencies(${test_executable} ${PROJECT_NAME} ${metatype_test})

add_test(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${PROJECT_NAME}Tests PROPERTY LABELS ${PROJECT_NAME})
