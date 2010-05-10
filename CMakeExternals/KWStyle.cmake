#
# KWStyle
#
SET(kwstyle_DEPENDS)
IF(CTK_USE_KWSTYLE)
  IF(NOT DEFINED CTK_KWSTYLE_EXECUTABLE)
    SET(proj KWStyle-CVSHEAD)
    SET(kwstyle_DEPENDS ${proj})
    ExternalProject_Add(${proj}
      LIST_SEPARATOR ${sep}
      CVS_REPOSITORY ":pserver:anoncvs:@public.kitware.com:/cvsroot/KWStyle"
      CVS_MODULE "KWStyle"
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
      )
    SET(CTK_KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)
  ENDIF()
ENDIF()