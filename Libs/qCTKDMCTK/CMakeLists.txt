CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(CTKDCMTK)

#-----------------------------------------------------------------------------
# Advanced options
#

#-----------------------------------------------------------------------------
# External libraries.  
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

INCLUDE(CMake/qCTKParseArgumentsMacro.cmake)
INCLUDE(CMake/qCTKBuildLibMacro.cmake)
  
#-----------------------------------------------------------------------------
# Build shared lib by default
#
OPTION(qCTK_BUILD_SHARED_LIBS "Build ${PROJECT_NAME} with shared libraries." ON)
SET(qCTK_LIBRARY_MODE "")
IF( qCTK_BUILD_SHARED_LIBS)
  SET(qCTK_LIBRARY_MODE "SHARED")
ENDIF(qCTK_BUILD_SHARED_LIBS)
MARK_AS_ADVANCED(qCTK_BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# Output directories.
#
IF(NOT LIBRARY_OUTPUT_PATH)
  SET(LIBRARY_OUTPUT_PATH ${CTKDCMTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${CTKDCMTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
SET(CTKDCMTK_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
SET(CTKDCMTK_EXECUTABLE_PATH ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
SET(qCTK_INSTALL_BIN_DIR "bin")
SET(qCTK_INSTALL_LIB_DIR "lib")
SET(qCTK_INSTALL_INCLUDE_DIR "include")
#SET(qCTK_INSTALL_DOC_DIR "doc")

#
# QT
#
FIND_PACKAGE(Qt4)
IF(QT4_FOUND)
  SET(QT_USE_QTNETWORK ON)
  INCLUDE(${QT_USE_FILE})
ELSE(QT4_FOUND)
  MESSAGE("Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
ENDIF(QT4_FOUND)

# Build the code Base  
ADD_SUBDIRECTORY(Base)

# Build the application(s)
ADD_SUBDIRECTORY(Applications)

