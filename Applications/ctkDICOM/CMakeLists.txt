PROJECT(ctkDICOM)

#
# See CTK/CMake/ctkMacroBuildQtApp.cmake for details
#
  
# Source files
SET(KIT_SRCS
  ctkDICOM.cxx
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Additional target libraries - Note that CTK_BASE_LIBRARIES is already included
SET(KIT_target_libraries
  )

# Additional directories to include - Not that CTK_INCLUDE_LIBRARIES is already included
SET(KIT_include_directories
  )

ctkMacroBuildQtApp(
  NAME ${PROJECT_NAME}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )

# Testing
IF(BUILD_TESTING)
#   ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
