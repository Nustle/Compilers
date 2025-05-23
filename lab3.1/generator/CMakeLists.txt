cmake_minimum_required(VERSION 3.19)
project(generator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

add_executable(generator main.cpp
        include
        src/lexer/scanner.cpp
        src/lexer/compiler.cpp
        src/lexer/position.cpp
        src/parser/parser.cpp
        src/parser/node.cpp
        src/parser/analyzer_table.cpp
        src/semantics/ast.cpp
        src/semantics/table_generator.cpp
)