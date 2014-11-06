#
# OS: Travis CI Linux
# Hardware: x86_64 GNU/Linux VM
# GPU: NA
#

# Note: The specific version and processor type of this machine should be reported in the 
# header above. Indeed, this file will be send to the dashboard as a NOTE file. 

cmake_minimum_required(VERSION 2.8)

#
# For additional information, see http://www.commontk.org/index.php/Dashboard_setup
#

set(CTK_QT_VERSION $ENV{CTK_QT_VERSION})
if(NOT CTK_QT_VERSION)
  set(CTK_QT_VERSION 4)
endif()

#
# Dashboard properties
#
set(MY_OPERATING_SYSTEM "Ubuntu-12.04")
set(MY_COMPILER "g++4.6.3")
set(CTEST_SITE "travis-ci") # for example: mymachine.kitware, mymachine.dkfz, ...
set(CTEST_DASHBOARD_ROOT "$ENV{HOME}/build/commontk")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

find_package(Qt${CTK_QT_VERSION} REQUIRED)
set(MY_QT_VERSION ${QT${CTK_QT_VERSION}_VERSION})

#
# Dashboard options
#
set(WITH_KWSTYLE FALSE)
set(WITH_MEMCHECK FALSE)
set(WITH_COVERAGE TRUE)
set(WITH_DOCUMENTATION FALSE)
#set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ) # for example: $ENV{HOME}/Projects/Doxygen
set(CTEST_BUILD_CONFIGURATION "Release")
set(CTEST_TEST_TIMEOUT 500)
set(CTEST_BUILD_FLAGS "-j4") # Use multiple CPU cores to build
set(CTEST_PARALLEL_LEVEL 4) # Number of tests running in parallel

# experimental: 
#     - run_ctest() macro will be called *ONE* time
#     - binary directory will *NOT* be cleaned
# continuous: 
#     - run_ctest() macro will be called EVERY 5 minutes ... 
#     - binary directory will *NOT* be cleaned
#     - configure/build will be executed *ONLY* if the repository has been updated
# nightly: 
#     - run_ctest() macro will be called *ONE* time
#     - binary directory *WILL BE* cleaned
set(SCRIPT_MODE "experimental") # "experimental", "continuous", "nightly"

#
# Project specific properties
#
set(CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/CTK")
set(CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/CTK-Superbuild-${CTEST_BUILD_CONFIGURATION}-${SCRIPT_MODE}")

# Additional CMakeCache options - For example:
# CTK_LIB_Widgets:BOOL=ON
# CTK_APP_ctkDICOM:BOOL=ON

set(ADDITIONNAL_CMAKECACHE_OPTION "
CTK_QT_VERSION:STRING=${CTK_QT_VERSION}
CTK_BUILD_ALL:BOOL=ON
CTK_BUILD_EXAMPLES:BOOL=ON
CTK_USE_SYSTEM_VTK:BOOL=ON
")

# List of test that should be explicitly disabled on this machine
set(TEST_TO_EXCLUDE_REGEX "")

# set any extra environment variables here
#set(ENV{DISPLAY} ":9")

find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
find_program(CTEST_GIT_COMMAND NAMES git)

#
# Git repository - Overwrite the default value provided by the driver script
#
# set(GIT_REPOSITORY http://github.com/YOURUSERNAME/CTK.git)

##########################################
# WARNING: DO NOT EDIT BEYOND THIS POINT #
##########################################

set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

#
# Project specific properties
#
set(CTEST_PROJECT_NAME "CTK")
set(CTEST_BUILD_NAME "${MY_OPERATING_SYSTEM}-${MY_COMPILER}-QT${MY_QT_VERSION}-${CTEST_BUILD_CONFIGURATION}")

#
# Display build info
#
message("site name: ${CTEST_SITE}")
message("build name: ${CTEST_BUILD_NAME}")
message("script mode: ${SCRIPT_MODE}")
message("coverage: ${WITH_COVERAGE}, memcheck: ${WITH_MEMCHECK}")

#
# Convenient macro allowing to download a file
#
macro(downloadFile url dest)
  file(DOWNLOAD ${url} ${dest} STATUS status)
  list(GET status 0 error_code)
  list(GET status 1 error_msg)
  if(error_code)
    message(FATAL_ERROR "error: Failed to download ${url} - ${error_msg}")
  endif()
endmacro()

#
# Download and include dashboard driver script 
#
set(url http://commontk.org/ctkDashboardDriverScript.cmake)
set(dest ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}.driver)
downloadfile(${url} ${dest})
include(${dest})

set(travis_url "/tmp/travis.url")
file(WRITE ${travis_url} "https://travis-ci.org/commontk/CTK/builds/$ENV{TRAVIS_BUILD_ID}")
ctest_upload(FILES ${travis_url})
file(REMOVE ${travis_url})
