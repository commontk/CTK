#
# QtMobility
#
SET(QtMobility_DEPENDS)
ctkMacroShouldAddExternalProject(QTMOBILITY_QTSERVICEFW_LIBRARIES add_project)
IF(${add_project})
  SET(proj QtMobility)
#   MESSAGE(STATUS "Adding project:${proj}")
  SET(QtMobility_DEPENDS ${proj})
  
  # Configure patch script
  SET(qtmobility_src_dir ${ep_source_dir}/${proj})
  SET(qtmobility_patch_dir ${CTK_SOURCE_DIR}/Utilities/QtMobility/)
  SET(qtmobility_configured_patch_dir ${CTK_BINARY_DIR}/Utilities/QtMobility/)
  SET(qtmobility_patchscript
    ${CTK_BINARY_DIR}/Utilities/QtMobility/QtMobility-1.0.0-patch.cmake)
  CONFIGURE_FILE(
    ${CTK_SOURCE_DIR}/Utilities/QtMobility/QtMobility-1.0.0-patch.cmake.in
    ${qtmobility_patchscript} @ONLY)

  # Define configure options
  SET(qtmobility_modules "serviceframework")
  SET(qtmobility_build_type "release")
  IF(UNIX)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      SET(qtmobility_build_type "debug")
    ENDIF()
  ELSEIF(NOT ${CMAKE_CFG_INTDIR} STREQUAL "Release")
    SET(qtmobility_build_type "debug")
  ENDIf()
  
  SET(qtmobility_make_cmd)
  IF(UNIX OR MINGW)
    SET(qtmobility_make_cmd make)
  ELSEIF(WIN32)
    SET(qtmobility_make_cmd nmake)
  ENDIF()

  ExternalProject_Add(${proj}
    URL ${CTK_SOURCE_DIR}/Utilities/QtMobility/qt-mobility-servicefw-opensource-src-1.0.0.tar.gz
    PATCH_COMMAND ${CMAKE_COMMAND} -P ${qtmobility_patchscript}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure -${qtmobility_build_type} -libdir ${CMAKE_BINARY_DIR}/CTK-build/bin -no-docs -modules ${qtmobility_modules}
    BUILD_COMMAND ${qtmobility_make_cmd}
    INSTALL_COMMAND ${qtmobility_make_cmd} install
    BUILD_IN_SOURCE 1
    )
ENDIF()