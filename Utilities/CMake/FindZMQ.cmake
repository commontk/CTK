# - Find an ZMQ installation or build tree.

# When ZMQ is found, the ZMQConfig.cmake file is sourced to setup the
# location and configuration of ZMQ.  Please read this file, or
# ZMQConfig.cmake.in from the ZMQ source tree for the full list of
# definitions.  Of particular interest is ZMQ_USE_FILE, a CMake source file
# that can be included to set the include directories, library directories,
# and preprocessor macros.  In addition to the variables read from
# ZMQConfig.cmake, this find module also defines
#  ZMQ_DIR  - The directory containing ZMQConfig.cmake.  
#             This is either the root of the build tree, 
#             or the CTK/CMakeExternals/Build/ZMQ directory.  
#             This is the only cache entry.
#   
#  ZMQ_FOUND - Whether ZMQ was found.  If this is true, 
#              ZMQ_DIR is okay.
#
#  USE_ZMQ_FILE - The full path to the ZMQ.cmake file.  
#                 This is provided for backward 
#                 compatability.  Use ZMQ_USE_FILE
#                 instead.


set(ZMQ_DIR_STRING "directory containing ZMQConfig.cmake.  This is either the root of the build tree, or PREFIX/lib/zmq for an installation.")

# Search only if the location is not already known.
if(NOT ZMQ_DIR)
  # Get the system search path as a list.
  if(UNIX)
    string(REGEX MATCHALL "[^:]+" ZMQ_DIR_SEARCH1 "$ENV{PATH}")
  else()
    string(REGEX REPLACE "\\\\" "/" ZMQ_DIR_SEARCH1 "$ENV{PATH}")
  endif()
  string(REGEX REPLACE "/;" ";" ZMQ_DIR_SEARCH2 ${ZMQ_DIR_SEARCH1})

  # Construct a set of paths relative to the system search path.
  set(ZMQ_DIR_SEARCH "")
  foreach(dir ${ZMQ_DIR_SEARCH2})
    set(ZMQ_DIR_SEARCH ${ZMQ_DIR_SEARCH} "${dir}/../lib/zmq")
  endforeach()

  #
  # Look for an installation or build tree.
  #
  find_path(ZMQ_DIR ZMQConfig.cmake
    # Look for an environment variable ZMQ_DIR.
    $ENV{ZMQ_DIR}

    # Look in places relative to the system executable search path.
    ${ZMQ_DIR_SEARCH}

    # Look in standard UNIX install locations.
    /usr/local/lib/zmq
    /usr/lib/zmq

    # Read from the CMakeSetup registry entries.  It is likely that
    # ZMQ will have been recently built.
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]

    # Help the user find it if we cannot.
    DOC "The ${ZMQ_DIR_STRING}"
  )
endif()

# If ZMQ was found, load the configuration file to get the rest of the
# settings.
if(ZMQ_DIR)
  set(ZMQ_FOUND 1)
  include(${ZMQ_DIR}/ZMQConfig.cmake)

  # Set USE_ZMQ_FILE for backward-compatability.
  set(USE_ZMQ_FILE ${ZMQ_USE_FILE})
else()
  set(ZMQ_FOUND 0)
  if(ZMQ_FIND_REQUIRED)
    message(FATAL_ERROR "Please set ZMQ_DIR to the ${ZMQ_DIR_STRING}")
  endif()
endif()
