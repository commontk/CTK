set(SRCS
  ctkMetaTypeImplTestMain.cpp
)

set(MOC_SRCS )

set(MY_MOC_CXX )

#QT4_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

set(test_executable ${PROJECT_NAME}CppTests)

set(${test_executable}_DEPENDENCIES ${fw_lib} ${fwtestutil_lib})

set(my_includes)
ctkFunctionGetIncludeDirs(my_includes ${test_executable})
include_directories(${my_includes})

add_executable(${test_executable} ${SRCS} ${MY_MOC_CXX})
target_link_libraries(${test_executable}
  ${${test_executable}_DEPENDENCIES}
)

add_dependencies(${test_executable} ${PROJECT_NAME} ${metatype_test})

add_test(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${PROJECT_NAME}Tests PROPERTY LABELS ${PROJECT_NAME})
