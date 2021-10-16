cmake_minimum_required(VERSION 3.10)
project(websocket_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread")
add_executable(websocket_server main.cpp)

target_link_libraries(websocket_server /usr/lib/x86_64-linux-gnu/libboost_system.so /usr/local/lib/libhiredis.so)
