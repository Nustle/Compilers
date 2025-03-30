cmake_minimum_required(VERSION 3.10)
project(lab1_5)

set(CMAKE_CXX_STANDARD 14)

find_package(FLEX REQUIRED)

FLEX_TARGET(Lexer main.l ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)

