cmake_minimum_required(VERSION 3.23)

project(Attendance VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)

add_subdirectory(Core)
add_subdirectory(Attendance)
