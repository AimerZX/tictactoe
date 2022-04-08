cmake_minimum_required(VERSION 3.19)
project(tictactoe)

set(CMAKE_CXX_STANDARD 14)

link_libraries(ws2_32)
add_executable(tictactoe main.cpp)