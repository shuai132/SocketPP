cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

include_directories(socketpp)

# socketpp library
add_subdirectory(socketpp)

# examples
add_subdirectory(examples)

# custom
add_custom_target(make_all cd ${CMAKE_BINARY_DIR} && make)
