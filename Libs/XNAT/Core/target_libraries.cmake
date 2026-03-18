#
# See CMake/ctkFunctionGetTargetLibraries.cmake
#
# This file should list the libraries required to build the current CTK libraries
#
if(CTK_QT_VERSION VERSION_EQUAL "5")
  set(target_libraries
    CTKCore
    qRestAPI_LIBRARIES
    QT_LIBRARIES
    QtScript
    )
else()
  set(target_libraries
    CTKCore
    qRestAPI_LIBRARIES
    QT_LIBRARIES
    )
endif()
