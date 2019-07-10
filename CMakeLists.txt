cmake_minimum_required(VERSION 3.1)

include_directories(socketpp socketpp/epoll)

add_subdirectory(socketpp)

add_subdirectory(examples)
