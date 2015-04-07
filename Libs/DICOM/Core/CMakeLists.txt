project(CTKDICOMCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_DICOM_CORE_EXPORT")

# determine the DCMTK version we're using
if(EXISTS ${DCMTK_config_INCLUDE_DIR}/cfunix.h)
  set(_DCMTK_CONFIG_HEADER ${DCMTK_config_INCLUDE_DIR}/cfunix.h)
elseif(EXISTS ${DCMTK_config_INCLUDE_DIR}/osconfig.h)
  set(_DCMTK_CONFIG_HEADER ${DCMTK_config_INCLUDE_DIR}/osconfig.h)
else()
  message(SEND_ERROR "DCMTK version cannot be determined!")
endif()

file(READ ${_DCMTK_CONFIG_HEADER} _DCMTK_CONFIG)
# simple check, we do not expect anything else
if(_DCMTK_CONFIG MATCHES "PACKAGE_VERSION_NUMBER 360")
  set(DCMTK_VERSION_IS_360 1)
  message(STATUS "Detected DCMTK 3.6.0, will include backported SCU")
endif()

# Source files
set(KIT_SRCS
  ctkDICOMAbstractThumbnailGenerator.cpp
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.cpp
  ctkDICOMDatabase.h
  ctkDICOMItem.h
  ctkDICOMFilterProxyModel.cpp
  ctkDICOMFilterProxyModel.h
  ctkDICOMIndexer.cpp
  ctkDICOMIndexer.h
  ctkDICOMIndexer_p.h
  ctkDICOMItem.cpp
  ctkDICOMItem.h
  ctkDICOMModel.cpp
  ctkDICOMModel.h
  ctkDICOMObjectModel.cpp
  ctkDICOMObjectModel.h
  ctkDICOMPersonName.cpp
  ctkDICOMPersonName.h
  ctkDICOMQuery.cpp
  ctkDICOMQuery.h
  ctkDICOMRetrieve.cpp
  ctkDICOMRetrieve.h
  ctkDICOMTester.cpp
  ctkDICOMTester.h
  ctkDICOMUtil.cpp
  ctkDICOMUtil.h
)

if(DCMTK_VERSION_IS_360)
  list(APPEND KIT_SRCS ctkDcmSCU.cc)
endif(DCMTK_VERSION_IS_360)


# Abstract class should not be wrapped !
set_source_files_properties(
  ctkDICOMAbstractThumbnailGenerator.h
  WRAP_EXCLUDE
  )

# XXX DCMTK 3.6.1: Remove the HAVE_DCMTK_LOG4CPLUS_LOGGER test when DCMTK 3.6.1
#     can be expected on all supported platforms.
# XXX On multi configuration system, there is no way to run 'try_compile` configure
#     tests based on the configuration that WILL be selected to build the project.
#     Since the try compile test 'HAVE_DCMTK_LOG4CPLUS_LOGGER' has originally been
#     added to be able to build against DCMTK 3.6.0 available by default on linux
#     distribution. Let's skip it in the case of multi configuration system and
#     assume DCMTK provides 'dcmtk::log4cplus::Logger'.
#     We also skip this test if DCMTK_CMAKE_DEBUG_POSTFIX is set, because then the
#     DCMTK_LIBRARIES variable will have a value similar to "debug <lib1> ..." which
#     will not link the DCMTK debug libraries in the try_compile call because
#     BUILD_TYPE cannot be set for the generated CMakeLists.txt file.
# TODO When build as external project, the config of the parent project could be passed to CTK
#      and used as a hint. We need to define what would be the convention for such hint.
if(DCMTK_CMAKE_DEBUG_POSTFIX OR (CMAKE_CONFIGURATION_TYPES AND NOT DEFINED HAVE_DCMTK_LOG4CPLUS_LOGGER))
  set(HAVE_DCMTK_LOG4CPLUS_LOGGER 1 CACHE INTERNAL "Test HAVE_DCMTK_LOG4CPLUS_LOGGER")
endif()
# Check if DCMTK provides 'dcmtk::log4cplus::Logger', if not fallback to 'log4cplus::Logger'.
# Indeed, following DCMTK-3.6.1_20110519 (commit e39d190), log4cplus has been moved
# into dcmtk::log4cplus namespace.
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_DEFINITIONS -DHAVE_CONFIG_H)
set(CMAKE_REQUIRED_INCLUDES ${DCMTK_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${DCMTK_LIBRARIES})
check_cxx_source_compiles(
  "#include <dcmtk/oflog/oflog.h>\nint main(int, char*[]){dcmtk::log4cplus::Logger logger; return 0;}"
  HAVE_DCMTK_LOG4CPLUS_LOGGER)
if(HAVE_DCMTK_LOG4CPLUS_LOGGER)
  set_source_files_properties(
    ctkDICOMUtil.cpp
    PROPERTIES COMPILE_DEFINITIONS HAVE_DCMTK_LOG4CPLUS_LOGGER
    )
endif()

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkDICOMAbstractThumbnailGenerator.h
  ctkDICOMDatabase.h
  ctkDICOMIndexer.h
  ctkDICOMIndexer_p.h
  ctkDICOMFilterProxyModel.h
  ctkDICOMModel.h
  ctkDICOMObjectModel.h
  ctkDICOMQuery.h
  ctkDICOMRetrieve.h
  ctkDICOMTester.h
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkDICOMCore.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::Sql Qt5::Gui)
endif()

# create a dcm query/retrieve service config file that points to the build dir
set (DCMQRSCP_STORE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing)
configure_file( Resources/dcmqrscp.cfg.in dcmqrscp.cfg )
set (DCMQRSCP_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/dcmqrscp.cfg)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

if(DEFINED DCMTK_HAVE_CONFIG_H_OPTIONAL AND NOT DCMTK_HAVE_CONFIG_H_OPTIONAL)
  # Workaround Debian packaging issue - See FindDCMTK.cmake for more details
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})
  if("${CMAKE_VERSION}" VERSION_GREATER 2.8.10)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_COMPILE_DEFINITIONS ${DCMTK_DEFINITIONS})
  endif()
endif()


if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Plugins
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
