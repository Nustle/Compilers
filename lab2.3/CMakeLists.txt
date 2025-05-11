cmake_minimum_required(VERSION 3.19)
project(lab_2_3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

add_executable(lab_2_3 main.cpp
        include
        src/scanner.cpp
        src/compiler.cpp
        src/position.cpp
        src/parser.cpp
        src/node.cpp
        )