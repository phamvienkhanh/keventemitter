cmake_minimum_required(VERSION 3.0.0)
project(kevent_emiter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(kevent_emiter test.cpp)
target_link_libraries(kevent_emiter pthread)

