cmake_minimum_required(VERSION 3.1)

link_libraries(socketpp pthread)

add_executable(echo_server echo_server.cpp)
add_executable(echo_client echo_client.cpp)
