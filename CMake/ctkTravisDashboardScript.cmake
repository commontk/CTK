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

#
# Dashboard properties
#
set(MY_OPERATING_SYSTEM "Ubuntu-12.04")
set(MY_COMPILER "g++4.6.3")
set(MY_QT_VERSION "4.8.1")
set(QT_QMAKE_EXECUTABLE "/usr/bin/qmake")
set(CTEST_SITE "travis-ci") # for example: mymachine.kitware, mymachine.dkfz, ...
set(CTEST_DASHBOARD_ROOT "/tmp")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

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

# Additionnal CMakeCache options - For example:
# CTK_LIB_Widgets:BOOL=ON
# CTK_APP_ctkDICOM:BOOL=ON

set(ADDITIONNAL_CMAKECACHE_OPTION "
CTK_USE_SYSTEM_DCMTK:BOOL=ON
#CTK_USE_SYSTEM_ITK:BOOL=ON
ITK_DIR:PATH=/usr/lib/InsightToolkit

CTK_APP_ctkCommandLineModuleExplorer:BOOL=ON
CTK_APP_ctkDICOM:BOOL=ON
CTK_APP_ctkDICOM2:BOOL=ON
CTK_APP_ctkDICOMHost:BOOL=ON
CTK_APP_ctkDICOMIndexer:BOOL=ON
CTK_APP_ctkExampleHost:BOOL=ON
CTK_APP_ctkExampleHostedApp:BOOL=ON
CTK_APP_ctkPluginBrowser:BOOL=ON
CTK_APP_ctkPluginGenerator:BOOL=ON
CTK_APP_ctkXnatTreeBrowser:BOOL=ON

CTK_LIB_CommandLineModules/Backend/FunctionPointer:BOOL=ON
CTK_LIB_CommandLineModules/Backend/LocalProcess:BOOL=ON
CTK_LIB_CommandLineModules/Backend/XMLChecker:BOOL=ON
CTK_LIB_CommandLineModules/Core:BOOL=ON
CTK_LIB_CommandLineModules/Frontend/QtGui:BOOL=ON
CTK_LIB_CommandLineModules/Frontend/QtWebKit:BOOL=ON

CTK_LIB_DICOM/Core:BOOL=ON
CTK_LIB_DICOM/Widgets:BOOL=ON

CTK_LIB_ImageProcessing/ITK/Core:BOOL=ON
CTK_LIB_PluginFramework:BOOL=ON
CTK_LIB_Widgets:BOOL=ON
CTK_LIB_XNAT/Core:BOOL=ON
CTK_LIB_XNAT/Widgets:BOOL=ON

CTK_PLUGIN_org.commontk.configadmin:BOOL=ON
CTK_PLUGIN_org.commontk.dah.cmdlinemoduleapp:BOOL=ON
CTK_PLUGIN_org.commontk.dah.core:BOOL=ON
CTK_PLUGIN_org.commontk.dah.exampleapp:BOOL=ON
CTK_PLUGIN_org.commontk.dah.examplehost:BOOL=ON
CTK_PLUGIN_org.commontk.dah.host:BOOL=ON
CTK_PLUGIN_org.commontk.dah.hostedapp:BOOL=ON
CTK_PLUGIN_org.commontk.eventadmin:BOOL=ON
CTK_PLUGIN_org.commontk.log:BOOL=ON
CTK_PLUGIN_org.commontk.metatype:BOOL=ON
CTK_PLUGIN_org.commontk.plugingenerator.core:BOOL=ON
CTK_PLUGIN_org.commontk.plugingenerator.ui:BOOL=ON
")

# List of test that should be explicitly disabled on this machine
set(TEST_TO_EXCLUDE_REGEX "")

# set any extra environment variables here
set(ENV{DISPLAY} ":0")

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
