
#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

MACRO(ctkMacroBuildQtPlugin)
  CtkMacroParseArguments(MY
    "NAME;EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES;LIBRARY_TYPE"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_NAME)
    MESSAGE(SEND_ERROR "NAME is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
    MESSAGE(SEND_ERROR "EXPORT_DIRECTIVE is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_LIBRARY_TYPE)
    SET(MY_LIBRARY_TYPE "SHARED")
  ENDIF()

  # Define library name
  SET(lib_name ${MY_NAME})

  # --------------------------------------------------------------------------
  # Include dirs
  SET(my_includes
    ${CTK_BASE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MY_INCLUDE_DIRECTORIES}
    )
  INCLUDE_DIRECTORIES(
    ${my_includes}
    )
 
  SET(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  SET(MY_EXPORT_HEADER_PREFIX ${MY_NAME})
  SET(MY_LIBNAME ${lib_name})
  
  CONFIGURE_FILE(
    ${CTK_SOURCE_DIR}/Libs/CTKExport.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  SET(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  QT4_WRAP_CPP(MY_SRCS ${MY_MOC_SRCS})
  QT4_WRAP_UI(MY_UI_CXX ${MY_UI_FORMS})
  SET(MY_QRC_SRCS "")
  IF(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_MOC_SRCS}
    ${MY_QRC_SRCS}
    ${MY_UI_CXX}
    )
  
  ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_UI_CXX}
    ${MY_QRC_SRCS}
    )

  # Note: The plugin may be installed in some other location ???
  # Install rules
# IF(CTK_BUILD_SHARED_LIBS)
# INSTALL(TARGETS ${lib_name}
# RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR} COMPONENT Runtime
# LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Runtime
# ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Development)
# ENDIF()
  
  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )
  TARGET_LINK_LIBRARIES(${lib_name} ${my_libs})
  
  # Install headers
  #FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  #INSTALL(FILES
  # ${headers}
  # ${dynamicHeaders}
  # DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  # )

ENDMACRO()

