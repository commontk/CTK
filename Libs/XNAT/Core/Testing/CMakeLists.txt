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

if(CTK_QT_VERSION VERSION_LESS "5")
  QT4_WRAP_CPP(KITTests_MOC_CPP ${KITTests_MOC_SRCS})
else()
  qt5_wrap_cpp(KITTests_MOC_CPP ${KITTests_MOC_SRCS})
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${KITTests_SRCS} ${KITTests_MOC_SRCS} ${KITTests_MOC_CPP})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

if(CTK_QT_VERSION VERSION_GREATER "4")
  target_link_libraries(${KIT}CppTests Qt5::Test)
endif()

SIMPLE_TEST(ctkXnatSessionTest)
