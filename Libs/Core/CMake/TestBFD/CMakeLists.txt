cmake_minimum_required(VERSION 2.8)

project(TestBFD)

# Sanity checks
if("${BFD_LIBRARY_NAME}" STREQUAL "")
  message(FATAL_ERROR "error: ${v} variable is an empty string !")
endif()

unset(BFD_LIBRARY CACHE)
find_library(BFD_LIBRARY ${BFD_LIBRARY_NAME})

ctk_add_executable_utf8(TestBFD TestBFD.cpp)
target_link_libraries(TestBFD ${BFD_LIBRARY})
