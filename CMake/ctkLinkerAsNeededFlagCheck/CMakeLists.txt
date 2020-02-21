include(${CTK_CMAKE_DIR}/ctkFunctionAddExecutableUtf8.cmake)

add_library(A SHARED A.cpp)
add_library(B SHARED B.cpp)
ctk_add_executable_utf8(C C.cpp)
target_link_libraries(C B A)
