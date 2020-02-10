set(KIT ${PROJECT_NAME})
set(LIBRARY_NAME ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkCmdLineModuleFrontendQtGuiTest.cpp
  ctkCmdLineModuleQtXslTransformTest.cpp
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(Tests_SRCS ${Tests_SRCS}
)

set(Tests_MOC_SRCS ${Tests_MOC_SRCS}
)

set(Tests_RESOURCES
  ctkCmdLineModuleFrontendQtGuiTestResources.qrc
)

include_directories(
  ${CMAKE_SOURCE_DIR}/Libs/Testing
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(Tests_MOC_CPP)
set(Tests_UI_CPP)
set(Tests_RESOURCES_SRCS)
if(CTK_QT_VERSION VERSION_GREATER "4")
  QT5_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
  QT5_GENERATE_MOCS(
    ctkCmdLineModuleFrontendQtGuiTest.cpp
    ctkCmdLineModuleQtXslTransformTest.cpp
    )
  if(TEST_UI_FORMS)
    QT5_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  QT5_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
else()
  QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
  QT4_GENERATE_MOCS(
    ctkCmdLineModuleFrontendQtGuiTest.cpp
    ctkCmdLineModuleQtXslTransformTest.cpp
    )
  if(TEST_UI_FORMS)
    QT4_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP} ${Tests_UI_CPP} ${Tests_RESOURCES_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

if(CTK_QT_VERSION VERSION_GREATER "4")
  target_link_libraries(${KIT}CppTests Qt5::Test)
endif()

#
# Add Tests
#

SIMPLE_TEST(ctkCmdLineModuleQtXslTransformTest)
SIMPLE_TEST(ctkCmdLineModuleFrontendQtGuiTest)
