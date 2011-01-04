# Code in this directory will be compiled into
# a utility library. Test executables may
# link to it to bootstrap a plugin framework and
# start registered tests.

SET(SRCS
  ctkPluginFrameworkTestUtil.cpp
  ctkPluginFrameworkTestRunner.cpp
  ctkTestSuiteInterface.h
)

SET(MOC_SRCS )

SET(MY_MOC_CXX )

#QT4_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

SET(lib_name ${fwtestutil_lib})

ADD_LIBRARY(${lib_name} SHARED ${SRCS} ${MY_MOC_CXX})
TARGET_LINK_LIBRARIES(${lib_name} ${fw_lib})

