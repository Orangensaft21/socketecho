cmake_minimum_required(VERSION 2.8)

project(socketecho)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(${PROJECT_NAME}
    "main.cpp"
    "sockethelper.h"
    "sockethelper.cpp"
    "echohandler.h"
    "echohandler.cpp"

    )
