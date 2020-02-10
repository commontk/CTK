#! Usage:
#! \code
#! ctk_add_executable_utf8(args)
#! \endcode
#!
#! This macro adds an executable that uses UTF-8 code page.
#!
#! Linux and Mac OSX already uses this code page by default but on Windows
#! it has to be set explicitly, by using a manifest file.
#!
#! \ingroup CMakeUtilities

function(ctk_add_executable_utf8)

if(WIN32)
  add_executable(${ARGN} ${CTK_CMAKE_DIR}/WindowsApplicationUseUtf8.manifest)
else()
  add_executable(${ARGN})
endif()

endfunction()
