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

QT4_WRAP_CPP(KITTests_MOC_CPP ${KITTests_MOC_SRCS})

add_executable(${KIT}Tests ${Tests} ${KITTests_SRCS} ${KITTests_MOC_SRCS} ${KITTests_MOC_CPP})
target_link_libraries(${KIT}Tests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

macro(SIMPLE_TEST TESTNAME)
  add_test(NAME ${TESTNAME} COMMAND ${KIT}Tests ${TESTNAME})
endmacro()

SIMPLE_TEST(ctkXnatSessionTest)
