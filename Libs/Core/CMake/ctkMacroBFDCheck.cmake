#
# ctkMacroBFDCheck.cmake - After this file is included into your main CMake script,
#                          HAVE_BFD will be defined if libbfd is available.
#

SET(BFD_LIBRARIES)
UNSET(HAVE_BFD)

IF(NOT WIN32)
  INCLUDE(CheckIncludeFile)
  CHECK_INCLUDE_FILE(bfd.h HAVE_BFD_HEADER)

  FIND_LIBRARY(BDF_LIBRARY bfd)

  IF(HAVE_BFD_HEADER AND BDF_LIBRARY)
    # make sure we can build with libbfd
    #MESSAGE(STATUS "Checking libbfd")
    TRY_COMPILE(HAVE_BFD
      ${CMAKE_CURRENT_BINARY_DIR}/CMake/TestBFD
      ${CMAKE_CURRENT_SOURCE_DIR}/CMake/TestBFD
      TestBFD
      CMAKE_FLAGS
      -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      OUTPUT_VARIABLE OUTPUT)
    #MESSAGE(${OUTPUT})

    IF(HAVE_BFD)
      SET(BFD_LIBRARIES ${BDF_LIBRARY})
      MESSAGE(STATUS "CTKCore: libbfd is available [${BDF_LIBRARY}]")
    ELSE()
      MESSAGE(STATUS "CTKCore: libbfd is *NOT* available")
    ENDIF()

  ENDIF()
ENDIF()
