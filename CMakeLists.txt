cmake_minimum_required(VERSION 3.10)
project(RegisterAllocation)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(regalloc
        src/Parser.cpp
        src/LiveRange.cpp
        src/Web.cpp
        src/GraphBuilder.cpp
        src/Graph.h
        src/main.cpp
        src/GraphColoring.cpp
        src/GraphColoring.h
        src/Menu.cpp
        src/output.cpp
)