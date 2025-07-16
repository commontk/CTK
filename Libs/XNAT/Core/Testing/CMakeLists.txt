set(KIT ${PROJECT_NAME})

set(KITTests_SRCS
  ctkXnatSessionTest.cpp
  )

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ${KITTests_SRCS}
  )

set(KITTests_MOC_SRCS
  ctkXnatSessionTest.h
  )

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${KITTests_SRCS} ${KITTests_MOC_SRCS} ${KITTests_MOC_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

target_link_libraries(${KIT}CppTests Qt${CTK_QT_VERSION}::Test)

SIMPLE_TEST(ctkXnatSessionTest)
