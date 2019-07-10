cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

link_libraries(socketpp pthread)

add_executable(echo_server echo_server.cpp)
