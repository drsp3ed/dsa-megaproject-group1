cmake_minimum_required(VERSION 4.0)
project(dsa_miniproject)

set(CMAKE_CXX_STANDARD 20)

add_executable(dsa_miniproject main.cpp
        DoublyLinkedList.cpp
        DoublyLinkedList.h
        BrowserHistory.cpp
        BrowserHistory.h)
