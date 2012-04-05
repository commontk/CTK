project(CTKTesting)

# CMake Macros
include(CMake/ctkMacroGenerateMocs.cmake)

install(FILES
  ctkTest.h
  DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
)
