set(SRCS
  ctkConfigAdminImplTestMain.cpp
)

set(MOC_SRCS )

set(MY_MOC_CXX )

#QT5_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

set(test_executable ${PROJECT_NAME}CppTests)

set(${test_executable}_DEPENDENCIES ${fw_lib} ${fwtestutil_lib} CTKCore)

# Ensure "ctkCompatibility_p.h" included from the export header is found
list(APPEND ${test_executable}_DEPENDENCIES CTKCore)

set(my_includes)
ctkFunctionGetIncludeDirs(my_includes ${test_executable})
include_directories(${my_includes})

ctk_add_executable_utf8(${test_executable} ${SRCS} ${MY_MOC_CXX})
target_link_libraries(${test_executable}
  ${${test_executable}_DEPENDENCIES}
)

add_dependencies(${test_executable} ${PROJECT_NAME} ${configadmin_test})

add_test(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${PROJECT_NAME}Tests PROPERTY LABELS ${PROJECT_NAME})
