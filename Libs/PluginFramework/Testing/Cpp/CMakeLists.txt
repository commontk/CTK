# Code in this directory will be compiled into
# a utility library. Test executables may
# link to it to bootstrap a plugin framework and
# start registered tests.

set(SRCS
  ctkPluginFrameworkTestUtil.cpp
  ctkPluginFrameworkTestRunner.cpp
  ctkTestSuiteInterface.h
)

set(MOC_SRCS )

set(MY_MOC_CXX )

#QT5_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

set(lib_name ${fwtestutil_lib})

add_library(${lib_name} SHARED ${SRCS} ${MY_MOC_CXX})
target_link_libraries(${lib_name} ${fw_lib})

target_link_libraries(${lib_name} Qt${CTK_QT_VERSION}::Test)
