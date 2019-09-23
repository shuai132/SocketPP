project(socketpp)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

set(SOCKET_PP_SRC
        socket_event.cpp
        Socket.cpp
        SocketServer.cpp
        SocketClient.cpp
        TCPStream.cpp
        TCPServer.cpp
        TCPClient.cpp
        )

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message("platform is Linux now!")
    set(SOCKET_PF_SRC
            epoll/epoll.cpp
            epoll/epoll_client.cpp
            platform/Socket_linux.cpp)
elseif(UNIX)
    message("platform is Unix now!")
    set(SOCKET_PF_SRC
            kqueue/kqueue.cpp
            platform/Socket_unix.cpp)
endif()

add_library(${PROJECT_NAME} ${SOCKET_PP_SRC} ${SOCKET_PF_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC .)
