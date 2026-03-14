cmake_minimum_required(VERSION 3.23)

project(Attendance VERSION 1.0.0)

option(BUILD_TESTING "Enable testing and build test" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")

if(WIN32)
    set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg.exe")
else()
    set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg")
endif()

if(NOT EXISTS ${VCPKG_EXECUTABLE})
    message(STATUS "Bootstrapping vcpkg...")

    if(WIN32)
        execute_process(
            COMMAND bootstrap-vcpkg.bat
            WORKING_DIRECTORY ${VCPKG_ROOT}
        )
    else()
        execute_process(
            COMMAND ./bootstrap-vcpkg.sh
            WORKING_DIRECTORY ${VCPKG_ROOT}
        )
    endif()
endif()

include(cmake/Dependencies.cmake)
include_directories(${CMAKE_SOURCE_DIR}/external)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(Tests)
endif()

add_subdirectory(Core)
add_subdirectory(Attendance)
