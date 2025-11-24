cmake_minimum_required(VERSION 3.10.0)
project(chessEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

add_executable(chess_engine ${SOURCES})


target_include_directories(chess_engine PRIVATE include)

# More aggressive optimizations
target_compile_options(chess_engine PRIVATE -O3 -march=native -flto)
#target_compile_options(chess_engine PRIVATE -O0 -g)