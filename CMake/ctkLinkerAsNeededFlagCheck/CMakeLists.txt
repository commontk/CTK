add_library(A SHARED A.cpp)
add_library(B SHARED B.cpp)
# No need to use ctk_add_executable_utf8 since there are
# no special characters are involved in this test.
add_executable(C C.cpp)
target_link_libraries(C B A)
