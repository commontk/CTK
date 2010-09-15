# - find DCMTK libraries and applications
#

#  DCMTK_INCLUDE_DIR   - Directories to include to use DCMTK
#  DCMTK_LIBRARIES     - Files to link against to use DCMTK
#  DCMTK_FOUND         - If false, don't try to use DCMTK
#  DCMTK_DIR           - (optional) Source directory for DCMTK
#
# DCMTK_DIR can be used to make it simpler to find the various include
# directories and compiled libraries if you've just compiled it in the
# source tree. Just set it to the root of the tree where you extracted
# the source (default to /usr/include/dcmtk/)

#=============================================================================
# Copyright 2004-2009 Kitware, Inc.
# Copyright 2009 Mathieu Malaterre <mathieu.malaterre@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

#
# Written for VXL by Amitha Perera.
# Upgraded for GDCM by Mathieu Malaterre.
# 

IF( NOT DCMTK_FOUND )
  SET( DCMTK_DIR "/usr/include/dcmtk/"
    CACHE PATH "Root of DCMTK source tree (optional)." )
  MARK_AS_ADVANCED( DCMTK_DIR )
ENDIF( NOT DCMTK_FOUND )

FIND_PATH( DCMTK_config_INCLUDE_DIR osconfig.h
  PATHS
    ${DCMTK_DIR}/config/include
    ${DCMTK_DIR}/config
    ${DCMTK_DIR}/include/dcmtk/config
    ${DCMTK_DIR}/include
  NO_DEFAULT_PATH
  
)

FIND_PATH( DCMTK_ofstd_INCLUDE_DIR ofstdinc.h
  PATHS
    ${DCMTK_DIR}/ofstd/include
    ${DCMTK_DIR}/ofstd
    ${DCMTK_DIR}/include/ofstd
    ${DCMTK_DIR}/include/dcmtk/ofstd
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_ofstd_LIBRARY ofstd
  PATHS
    ${DCMTK_DIR}/ofstd/libsrc
    ${DCMTK_DIR}/ofstd/libsrc/Release
    ${DCMTK_DIR}/ofstd/libsrc/Debug
    ${DCMTK_DIR}/ofstd/Release
    ${DCMTK_DIR}/ofstd/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_PATH( DCMTK_oflog_INCLUDE_DIR logger.h
  PATHS
    ${DCMTK_DIR}/oflog/include
    ${DCMTK_DIR}/oflog
    ${DCMTK_DIR}/include/oflog
    ${DCMTK_DIR}/include/dcmtk/oflog
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_oflog_LIBRARY oflog
  PATHS
    ${DCMTK_DIR}/oflog/libsrc
    ${DCMTK_DIR}/oflog/libsrc/Release
    ${DCMTK_DIR}/oflog/libsrc/Debug
    ${DCMTK_DIR}/oflog/Release
    ${DCMTK_DIR}/oflog/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_PATH( DCMTK_dcmdata_INCLUDE_DIR dctypes.h
  PATHS
    ${DCMTK_DIR}/include/dcmdata
    ${DCMTK_DIR}/include/dcmtk/dcmdata
    ${DCMTK_DIR}/dcmdata
    ${DCMTK_DIR}/dcmdata/include
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_dcmdata_LIBRARY dcmdata
  PATHS
    ${DCMTK_DIR}/dcmdata/libsrc
    ${DCMTK_DIR}/dcmdata/libsrc/Release
    ${DCMTK_DIR}/dcmdata/libsrc/Debug
    ${DCMTK_DIR}/dcmdata/Release
    ${DCMTK_DIR}/dcmdata/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_PATH( DCMTK_dcmjpeg_INCLUDE_DIR djdecode.h
  PATHS
    ${DCMTK_DIR}/include/dcmjpeg
    ${DCMTK_DIR}/include/dcmtk/dcmjpeg
    ${DCMTK_DIR}/dcmjpeg
    ${DCMTK_DIR}/dcmjpeg/include
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_dcmjpeg_LIBRARY dcmjpeg
  PATHS
    ${DCMTK_DIR}/dcmjpeg/libsrc
    ${DCMTK_DIR}/dcmjpeg/libsrc/Release
    ${DCMTK_DIR}/dcmjpeg/libsrc/Debug
    ${DCMTK_DIR}/dcmjpeg/Release
    ${DCMTK_DIR}/dcmjpeg/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_ijg12_LIBRARY ijg12
  PATHS
    ${DCMTK_DIR}/dcmjpeg/libsrc
    ${DCMTK_DIR}/dcmjpeg/libsrc/Release
    ${DCMTK_DIR}/dcmjpeg/libsrc/Debug
    ${DCMTK_DIR}/dcmjpeg/Release
    ${DCMTK_DIR}/dcmjpeg/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_ijg16_LIBRARY ijg16
  PATHS
    ${DCMTK_DIR}/dcmjpeg/libsrc
    ${DCMTK_DIR}/dcmjpeg/libsrc/Release
    ${DCMTK_DIR}/dcmjpeg/libsrc/Debug
    ${DCMTK_DIR}/dcmjpeg/Release
    ${DCMTK_DIR}/dcmjpeg/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_ijg8_LIBRARY ijg8
  PATHS
    ${DCMTK_DIR}/dcmjpeg/libsrc
    ${DCMTK_DIR}/dcmjpeg/libsrc/Release
    ${DCMTK_DIR}/dcmjpeg/libsrc/Debug
    ${DCMTK_DIR}/dcmjpeg/Release
    ${DCMTK_DIR}/dcmjpeg/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_PATH( DCMTK_dcmnet_INCLUDE_DIR dimse.h
  PATHS
    ${DCMTK_DIR}/include/dcmnet
    ${DCMTK_DIR}/include/dcmtk/dcmnet
    ${DCMTK_DIR}/dcmnet
    ${DCMTK_DIR}/dcmnet/include
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_dcmnet_LIBRARY dcmnet
  PATHS
    ${DCMTK_DIR}/dcmnet/libsrc
    ${DCMTK_DIR}/dcmnet/libsrc/Release
    ${DCMTK_DIR}/dcmnet/libsrc/Debug
    ${DCMTK_DIR}/dcmnet/Release
    ${DCMTK_DIR}/dcmnet/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

FIND_PATH( DCMTK_dcmimgle_INCLUDE_DIR dcmimage.h
  PATHS
    ${DCMTK_DIR}/dcmimgle/include
    ${DCMTK_DIR}/dcmimgle
    ${DCMTK_DIR}/include/dcmimgle
    ${DCMTK_DIR}/include/dcmtk/dcmimgle
  NO_DEFAULT_PATH
)

FIND_LIBRARY( DCMTK_dcmimgle_LIBRARY dcmimgle
  PATHS
    ${DCMTK_DIR}/dcmimgle/libsrc
    ${DCMTK_DIR}/dcmimgle/libsrc/Release
    ${DCMTK_DIR}/dcmimgle/libsrc/Debug
    ${DCMTK_DIR}/dcmimgle/Release
    ${DCMTK_DIR}/dcmimgle/Debug
    ${DCMTK_DIR}/lib
  NO_DEFAULT_PATH
)

# MM: I could not find this library on debian system / dcmtk 3.5.4
# Michael Onken: this module is now called dcmqrdb. I will re-work that script soon...
FIND_LIBRARY(DCMTK_imagedb_LIBRARY imagedb
  PATHS
    ${DCMTK_DIR}/imagectn/libsrc/Release
    ${DCMTK_DIR}/imagectn/libsrc/
    ${DCMTK_DIR}/imagectn/libsrc/Debug
  NO_DEFAULT_PATH
  )

IF( DCMTK_config_INCLUDE_DIR 
    AND DCMTK_ofstd_INCLUDE_DIR 
    AND DCMTK_ofstd_LIBRARY
    AND DCMTK_oflog_INCLUDE_DIR    
    AND DCMTK_oflog_LIBRARY
    AND DCMTK_dcmdata_INCLUDE_DIR
    AND DCMTK_dcmdata_LIBRARY
    AND DCMTK_dcmjpeg_INCLUDE_DIR
    AND DCMTK_dcmjpeg_LIBRARY
    AND DCMTK_dcmnet_INCLUDE_DIR
    AND DCMTK_dcmnet_LIBRARY    
    AND DCMTK_dcmimgle_INCLUDE_DIR
    AND DCMTK_dcmimgle_LIBRARY )

#   # Wrap library is required on Linux
#   IF(NOT WIN32)
#     FIND_LIBRARY(DCMTK_wrap_LIBRARY wrap)
#     MESSAGE(DCMTK_wrap_LIBRARY:${DCMTK_wrap_LIBRARY})
#     IF(NOT DCMTK_wrap_LIBRARY)
#       MESSAGE(FATAL_ERROR "error: Wrap library is required to use DCMTK. "
#                           "On Ubuntu, you could install it using 'sudo apt-get libwrap0'")
#     ENDIF()
#   ENDIF()
  
  SET( DCMTK_FOUND "YES" )
  SET( DCMTK_INCLUDE_DIR
    ${DCMTK_DIR}/include
    ${DCMTK_config_INCLUDE_DIR}
    ${DCMTK_ofstd_INCLUDE_DIR}
    ${DCMTK_oflog_INCLUDE_DIR}    
    ${DCMTK_dcmdata_INCLUDE_DIR}
    ${DCMTK_dcmjpeg_INCLUDE_DIR}
    ${DCMTK_dcmnet_INCLUDE_DIR}
    ${DCMTK_dcmimgle_INCLUDE_DIR}
  )

  SET( DCMTK_LIBRARIES
    ${DCMTK_dcmimgle_LIBRARY}
    ${DCMTK_dcmnet_LIBRARY}    
    ${DCMTK_dcmjpeg_LIBRARY}
    ${DCMTK_dcmdata_LIBRARY}
    ${DCMTK_ijg8_LIBRARY}
    ${DCMTK_ijg12_LIBRARY}
    ${DCMTK_ijg16_LIBRARY}
    ${DCMTK_oflog_LIBRARY}    
    ${DCMTK_ofstd_LIBRARY}
    ${DCMTK_config_LIBRARY}
  )

  IF(DCMTK_imagedb_LIBRARY)
   SET( DCMTK_LIBRARIES
   ${DCMTK_LIBRARIES}
   ${DCMTK_imagedb_LIBRARY}
   )
  ENDIF(DCMTK_imagedb_LIBRARY)

  IF( WIN32 )
    SET( DCMTK_LIBRARIES ${DCMTK_LIBRARIES} ws2_32 netapi32 wsock32)
  ENDIF( WIN32 )
  
#   IF (NOT WIN32)
#     SET( DCMTK_LIBRARIES ${DCMTK_LIBRARIES} ${DCMTK_wrap_LIBRARY} )
#   ENDIF()

ENDIF()

FIND_PROGRAM(DCMTK_DCMDUMP_EXECUTABLE dcmdump
  PATHS
    ${DCMTK_DIR}/bin
  NO_DEFAULT_PATH
  )

FIND_PROGRAM(DCMTK_DCMDJPEG_EXECUTABLE dcmdjpeg
  PATHS
    ${DCMTK_DIR}/bin
  NO_DEFAULT_PATH
  )

FIND_PROGRAM(DCMTK_DCMDRLE_EXECUTABLE dcmdrle
  PATHS
    ${DCMTK_DIR}/bin
  NO_DEFAULT_PATH
  )

FIND_PROGRAM(DCMTK_DCMQRSCP_EXECUTABLE dcmqrscp
  PATHS
    ${DCMTK_DIR}/bin
  NO_DEFAULT_PATH
  )

FIND_PROGRAM(DCMTK_STORESCU_EXECUTABLE storescu
  PATHS
    ${DCMTK_DIR}/bin
  NO_DEFAULT_PATH
  )

MARK_AS_ADVANCED(
  DCMTK_DCMDUMP_EXECUTABLE
  DCMTK_DCMDJPEG_EXECUTABLE
  DCMTK_DCMDRLE_EXECUTABLE
  DCMTK_DCMQRSCP_EXECUTABLE
  DCMTK_STORESCU_EXECUTABLE
  DCMTK_config_INCLUDE_DIR
  DCMTK_dcmdata_INCLUDE_DIR
  DCMTK_dcmdata_LIBRARY
  DCMTK_dcmnet_INCLUDE_DIR
  DCMTK_dcmnet_LIBRARY
  DCMTK_dcmimgle_INCLUDE_DIR
  DCMTK_dcmimgle_LIBRARY
  DCMTK_imagedb_LIBRARY 
  DCMTK_ofstd_INCLUDE_DIR
  DCMTK_ofstd_LIBRARY
  DCMTK_oflog_INCLUDE_DIR
  DCMTK_oflog_LIBRARY
  )

