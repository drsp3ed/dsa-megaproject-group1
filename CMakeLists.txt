cmake_minimum_required(VERSION 3.20...4.0)
project(dsa_miniproject)

set(CMAKE_CXX_STANDARD 20)

add_executable(dsa_miniproject main.cpp
        DoublyLinkedList.cpp
        DoublyLinkedList.h
        BrowserHistory.cpp
        BrowserHistory.h
        nlohmann/json.hpp)

target_include_directories(dsa_miniproject PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})