cmake_minimum_required(VERSION 3.19)
project(lab2_4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)
find_package(Boost 1.88.0 REQUIRED COMPONENTS json)

add_executable(lab2_4 main.cpp
        include
        src/scanner.cpp
        src/compiler.cpp
        src/position.cpp
        src/parser.cpp
        src/node.cpp
        )

target_include_directories(lab2_4 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(lab2_4 PRIVATE Boost::json)