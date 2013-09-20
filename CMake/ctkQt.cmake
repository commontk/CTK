set(CTK_QT_VERSION "4" CACHE STRING "Expected Qt version")
mark_as_advanced(CTK_QT_VERSION)

set_property(CACHE CTK_QT_VERSION PROPERTY STRINGS 4 5)

if(NOT (CTK_QT_VERSION VERSION_EQUAL "4" OR CTK_QT_VERSION VERSION_EQUAL "5"))
  message(FATAL_ERROR "Expected value for CTK_QT_VERSION is either '4' or '5'")
endif()
