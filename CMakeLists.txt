cmake_minimum_required(VERSION 3.10)
project(RegisterAllocation)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(regalloc
        src/Parser.cpp
        src/LiveRange.cpp
)