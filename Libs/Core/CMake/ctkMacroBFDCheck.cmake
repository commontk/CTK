#
# ctkMacroBFDCheck.cmake - After this file is included into your main CMake script,
#                          HAVE_BFD will be defined if libbfd is available.
#

if(CTK_LIB_Core_WITH_BFD_STATIC AND CTK_LIB_Core_WITH_BFD_SHARED)
  message(FATAL_ERROR "error: Options WITH_BFD_STATIC and WITH_BFD_SHARED are mutually exclusive ! "
                      "hint: Enable either WITH_BFD_STATIC or WITH_BFD_SHARED.")
endif()

if(NOT CTK_BUILD_SHARED_LIBS AND CTK_LIB_Core_WITH_BFD_SHARED)
  message(FATAL_ERROR "error: Options CTK_BUILD_SHARED_LIBS and WITH_BFD_STATIC are mutually exclusive ! "
                      "hint: Disable WITH_BFD_SHARED and enable WITH_BFD_STATIC if needed.")
endif()

set(BFD_LIBRARIES)
unset(HAVE_BFD CACHE)

set(TestBFD_BUILD_LOG "${CMAKE_CURRENT_BINARY_DIR}/CMake/TestBFD-build-log.txt")

if(CTK_LIB_Core_WITH_BFD_STATIC OR CTK_LIB_Core_WITH_BFD_SHARED)
  if(WIN32)
    message(FATAL_ERROR "error: Options WITH_BFD_STATIC or WITH_BFD_SHARED are not support on Windows !")
  endif()

  if(NOT WIN32)
    include(CheckIncludeFile)
    CHECK_INCLUDE_file(bfd.h HAVE_BFD_HEADER)
    if(NOT HAVE_BFD_HEADER)
      file(WRITE ${TestBFD_BUILD_LOG} "Could *NOT* find the required header file: bfd.h")
    endif()

    set(BFD_LIBRARY_NAME libbfd.a)
    set(TestBFD_LIBRARY_MODE STATIC)
    if(CTK_LIB_Core_WITH_BFD_SHARED)
      set(BFD_LIBRARY_NAME libbfd${CMAKE_SHARED_LIBRARY_SUFFIX})
      set(TestBFD_LIBRARY_MODE SHARED)
    endif()
    unset(BFD_LIBRARY CACHE)
    find_library(BFD_LIBRARY ${BFD_LIBRARY_NAME})
    if(NOT BFD_LIBRARY)
      file(WRITE ${TestBFD_BUILD_LOG} "Could *NOT* find the required bfd library: ${BFD_LIBRARY_NAME}")
    endif()

    if(HAVE_BFD_HEADER AND BFD_LIBRARY)
      # make sure we can build with libbfd
      #message(STATUS "Checking libbfd")
      try_compile(HAVE_BFD
        ${CMAKE_CURRENT_BINARY_DIR}/CMake/TestBFD
        ${CMAKE_CURRENT_SOURCE_DIR}/CMake/TestBFD
        TestBFD
        CMAKE_FLAGS
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBFD_LIBRARY_NAME:STRING=${BFD_LIBRARY_NAME}
        OUTPUT_VARIABLE OUTPUT)
      file(WRITE ${TestBFD_BUILD_LOG} ${OUTPUT})
      #message(${OUTPUT})

      if(HAVE_BFD)
        set(BFD_LIBRARIES ${BFD_LIBRARY})
      endif()
    endif()
  endif()

endif()

if(CTK_LIB_Core_WITH_BFD_SHARED AND NOT HAVE_BFD)
  message(WARNING "warning: CTKCore: Failed to enable BFD support. Disabling CTKCore WITH_BFD_SHARED library option.  "
                  "See ${TestBFD_BUILD_LOG} for more details.")
  set(CTK_LIB_Core_WITH_BFD_SHARED OFF CACHE BOOL "Enable CTKCore Library WITH_BFD_SHARED option" FORCE)
endif()
if(CTK_LIB_Core_WITH_BFD_STATIC AND NOT HAVE_BFD)
  message(WARNING "warning: CTKCore: Failed to enable BFD support. Disabling CTKCore WITH_BFD_STATIC library option. "
                  "See ${TestBFD_BUILD_LOG} for more details.")
  set(CTK_LIB_Core_WITH_BFD_STATIC OFF CACHE BOOL "Enable CTKCore Library WITH_BFD_STATIC option" FORCE)
endif()

if(HAVE_BFD)
  message(STATUS "CTKCore: BFD support enabled [${BFD_LIBRARIES}]")
else()
  message(STATUS "CTKCore: BFD support disabled")
endif()
