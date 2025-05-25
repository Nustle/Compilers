cmake_minimum_required(VERSION 3.19)
project(calculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

add_executable(calculator main.cpp
        include
        src/lexer/compiler.cpp
        src/lexer/position.cpp
        src/lexer/scanner.cpp
        src/parser/table.cpp
        src/parser/node.cpp
        src/parser/parser.cpp
        src/semantics/interpret.cpp
)