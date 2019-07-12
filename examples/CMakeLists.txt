cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server socketpp pthread)
