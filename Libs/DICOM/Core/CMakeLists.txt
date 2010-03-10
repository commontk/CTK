PROJECT(CTKDICOMCore)

#
# See CTK/CMake/CTKBuildQtLibMacro.cmake for details
#

SET(KIT_export_directive "Q_CTK_DICOM_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  )
  
# Source files
SET(KIT_SRCS
  qCTKDCMTK.cxx
  qCTKDCMTK.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  qCTKDCMTK.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
  Resources/dicom.qrc
)

# Additional Target libraries (QT libs already included)
SET(KIT_target_libraries
  CTKCore
  )

ctk_build_qtlib(
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
  #ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
