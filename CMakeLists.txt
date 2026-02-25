cmake_minimum_required(VERSION 3.23)

project(Attendance VERSION 1.0.0)

option(BUILD_TESTING "Enable testing and build test" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)
include_directories(${CMAKE_SOURCE_DIR}/external)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(Tests)
endif()

add_subdirectory(Core)
add_subdirectory(Attendance)
