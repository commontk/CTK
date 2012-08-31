project(CTKTesting)

if(CTK_USE_QTTESTING)
  include(../QtTesting/CMake/ctkQtTesting.cmake)
endif()
install(FILES
  ctkTest.h
  DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
)
