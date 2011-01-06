SET(SRCS
  ctkConfigAdminImplTestMain.cpp
)

SET(MOC_SRCS )

SET(MY_MOC_CXX )

#QT4_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

SET(test_executable ${PROJECT_NAME}CppTests)

ADD_EXECUTABLE(${test_executable} ${SRCS} ${MY_MOC_CXX})
TARGET_LINK_LIBRARIES(${test_executable}
  ${fw_lib}
  ${fwtestutil_lib}
)

ADD_DEPENDENCIES(${test_executable} ${PROJECT_NAME} ${configadmin_test})

ADD_TEST(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})
SET_PROPERTY(TEST ${PROJECT_NAME}Tests PROPERTY LABELS ${PROJECT_NAME})
