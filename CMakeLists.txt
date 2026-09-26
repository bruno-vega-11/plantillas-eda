cmake_minimum_required(VERSION 3.31)
project(Teoria)

set(CMAKE_CXX_STANDARD 20)

add_executable(Teoria main.cpp
        binaryHeap.cpp
        binomialHeap.cpp
        fibonacciHeap.cpp)
add_executable(persistencia_one persistentStack.cpp)