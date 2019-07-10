project(socketpp)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

aux_source_directory(./lib SOCKET_LOG_LIB_SRC)

add_library(${PROJECT_NAME} ${SOCKET_LOG_LIB_SRC}
        SocketPP.cpp
        TCPStream.cpp
        epoll_socket_bridge.cpp
        epoll/epoll.c
        )
