# Code in this directory will be compiled into
# a static utility library. Test executables may
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

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

ADD_LIBRARY(${lib_name} STATIC ${SRCS} ${MY_MOC_CXX})
TARGET_LINK_LIBRARIES(${lib_name} ${fw_lib})
