cmake_minimum_required(VERSION 3.10.0)
project(chessEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "src/*.cpp")

add_executable(chess_engine ${SOURCES})

target_include_directories(chess_engine PRIVATE include)

target_compile_options(chess_engine PRIVATE
    -O3
    -march=native
)

#set_target_properties(chess_engine PROPERTIES
  #  CXX_CLANG_TIDY "clang-tidy"
#)

# Debug alternative
# target_compile_options(chess_engine PRIVATE -O0 -g)
