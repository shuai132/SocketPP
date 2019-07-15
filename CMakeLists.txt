cmake_minimum_required(VERSION 3.1)

# socketpp library
add_subdirectory(socketpp)
target_include_directories(socketpp PRIVATE socketpp socketpp/epoll socketpp/kqueue)

# examples
add_subdirectory(examples)
target_include_directories(echo_server PRIVATE socketpp)
target_include_directories(client_string PRIVATE client socketpp)

# custom
add_custom_target(make_all cd ${CMAKE_BINARY_DIR} && make)
