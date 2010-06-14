#  Log4Qt_INCLUDE_DIR   - Directories to include to use log4qt
#  Log4Qt_LIBRARIES     - Files to link against to use log4qt
#  Log4Qt_FOUND         - If false, don't try to use log4qt

IF( NOT Log4Qt_FOUND )
  SET( Log4Qt_DIR "/usr/include/log4qt/"
    CACHE PATH "Root of Log4Qt source tree (optional)." )
  MARK_AS_ADVANCED(Log4Qt_DIR)
ENDIF()

FIND_PATH(Log4Qt_category_INCLUDE_DIR log4qt.h
  PATHS
    ${Log4Qt_DIR}/include/log4qt
  NO_DEFAULT_PATH
)

FIND_LIBRARY(Log4Qt_LIBRARIES Log4Qt
  PATHS
    ${Log4Qt_DIR}/
    ${Log4Qt_DIR}/Release
    ${Log4Qt_DIR}/Debug
    ${Log4Qt_DIR}/lib/Release
    ${Log4Qt_DIR}/lib/Debug
    ${Log4Qt_DIR}/lib
  NO_DEFAULT_PATH
)

if(Log4Qt_category_INCLUDE_DIR AND Log4Qt_LIBRARIES)
  set(Log4Qt_INCLUDE_DIR ${Log4Qt_DIR}/include ${Log4Qt_DIR}/src)
  set(Log4Qt_FOUND "YES" )
endif()
