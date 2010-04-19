PROJECT(CTKDICOMCore)

#
# 3rd party dependencies
#

FIND_PACKAGE(DCMTK)
IF(NOT DCMTK_FOUND)
  MESSAGE(FATAL_ERROR "error: DCMTK package is required to build ${PROJECT_NAME}")
ENDIF()

#
# See CTK/CMake/ctkMacroBuildQtLib.cmake for details
#

SET(KIT_export_directive "CTK_DICOM_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${DCMTK_INCLUDE_DIR}
  )
  
# Source files
SET(KIT_SRCS
  ctkDICOM.cpp
  ctkDICOM.h
  ctkDICOMIndexer.cpp
  ctkDICOMIndexer.h
  ctkDICOMModel.cpp
  ctkDICOMModel.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkDICOM.h
  ctkDICOMModel.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
  Resources/dicom.qrc
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildQtLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
