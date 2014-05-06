add_library(A SHARED A.cpp)
add_library(B SHARED B.cpp)
add_executable(C C.cpp)
target_link_libraries(C B A)
