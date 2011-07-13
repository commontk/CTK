#
# ctkMacroBFDCheck.cmake - After this file is included into your main CMake script,
#                          HAVE_BFD will be defined if libbfd is available.
#

IF(CTK_LIB_Core_WITH_BFD_STATIC AND CTK_LIB_Core_WITH_BFD_SHARED)
  MESSAGE(FATAL_ERROR "error: Options WITH_BFD_STATIC and WITH_BFD_SHARED are mutually exclusive ! "
                      "hint: Enable either WITH_BFD_STATIC or WITH_BFD_SHARED.")
ENDIF()

IF(NOT CTK_BUILD_SHARED_LIBS AND CTK_LIB_Core_WITH_BFD_SHARED)
  MESSAGE(FATAL_ERROR "error: Options CTK_BUILD_SHARED_LIBS and WITH_BFD_STATIC are mutually exclusive ! "
                      "hint: Disable WITH_BFD_SHARED and enable WITH_BFD_STATIC if needed.")
ENDIF()

SET(BFD_LIBRARIES)
UNSET(HAVE_BFD CACHE)

SET(TestBFD_BUILD_LOG "${CMAKE_CURRENT_BINARY_DIR}/CMake/TestBFD-build-log.txt")

if(CTK_LIB_Core_WITH_BFD_STATIC OR CTK_LIB_Core_WITH_BFD_SHARED)
  IF(WIN32)
    MESSAGE(FATAL_ERROR "error: Options WITH_BFD_STATIC or WITH_BFD_SHARED are not support on Windows !")
  ENDIF()

  IF(NOT WIN32)
    INCLUDE(CheckIncludeFile)
    CHECK_INCLUDE_FILE(bfd.h HAVE_BFD_HEADER)

    SET(BFD_LIBRARY_NAME libbfd.a)
    SET(TestBFD_LIBRARY_MODE STATIC)
    IF(CTK_LIB_Core_WITH_BFD_SHARED)
      SET(BFD_LIBRARY_NAME libbfd${CMAKE_SHARED_LIBRARY_SUFFIX})
      SET(TestBFD_LIBRARY_MODE SHARED)
    ENDIF()
    UNSET(BFD_LIBRARY CACHE)
    FIND_LIBRARY(BFD_LIBRARY ${BFD_LIBRARY_NAME})

    IF(HAVE_BFD_HEADER AND BFD_LIBRARY)
      # make sure we can build with libbfd
      #MESSAGE(STATUS "Checking libbfd")
      TRY_COMPILE(HAVE_BFD
        ${CMAKE_CURRENT_BINARY_DIR}/CMake/TestBFD
        ${CMAKE_CURRENT_SOURCE_DIR}/CMake/TestBFD
        TestBFD
        CMAKE_FLAGS
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBFD_LIBRARY_NAME:STRING=${BFD_LIBRARY_NAME}
        OUTPUT_VARIABLE OUTPUT)
      FILE(WRITE ${TestBFD_BUILD_LOG} ${OUTPUT})
      #MESSAGE(${OUTPUT})

      IF(HAVE_BFD)
        SET(BFD_LIBRARIES ${BFD_LIBRARY})
      ENDIF()
    ENDIF()
  ENDIF()

ENDIF()

IF(CTK_LIB_Core_WITH_BFD_SHARED AND NOT HAVE_BFD)
  MESSAGE(WARNING "warning: CTKCore: Failed to enable BFD support. Disabling CTKCore WITH_BFD_SHARED library option.  "
                  "See ${TestBFD_BUILD_LOG} for more details.")
  SET(CTK_LIB_Core_WITH_BFD_SHARED OFF CACHE BOOL "Enable CTKCore Library WITH_BFD_SHARED option" FORCE)
ENDIF()
IF(CTK_LIB_Core_WITH_BFD_STATIC AND NOT HAVE_BFD)
  MESSAGE(WARNING "warning: CTKCore: Failed to enable BFD support. Disabling CTKCore WITH_BFD_STATIC library option. "
                  "See ${TestBFD_BUILD_LOG} for more details.")
  SET(CTK_LIB_Core_WITH_BFD_STATIC OFF CACHE BOOL "Enable CTKCore Library WITH_BFD_STATIC option" FORCE)
endif()

IF(HAVE_BFD)
  MESSAGE(STATUS "CTKCore: BFD support enabled [${BFD_LIBRARIES}]")
ELSE()
  MESSAGE(STATUS "CTKCore: BFD support disabled")
ENDIF()
