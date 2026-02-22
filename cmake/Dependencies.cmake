# cmake/Dependencies.cmake

set(SODIUM_TARGET "")

if(WIN32)
    find_package(unofficial-sodium CONFIG QUIET)
    if(TARGET unofficial-sodium::sodium)
        set(SODIUM_TARGET unofficial-sodium::sodium)
    endif()
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SODIUM QUIET libsodium)
        if(SODIUM_FOUND)
            add_library(sodium INTERFACE)
            target_include_directories(sodium INTERFACE ${SODIUM_INCLUDE_DIRS})
            target_link_libraries(sodium INTERFACE ${SODIUM_LIBRARIES})
            set(SODIUM_TARGET sodium)
        endif()
    endif()
endif()

if(NOT SODIUM_TARGET)
    if(EXISTS ${CMAKE_SOURCE_DIR}/external/libsodium/CMakeLists.txt)
        add_subdirectory(external/libsodium)
        set(SODIUM_TARGET sodium)
    else()
        message(FATAL_ERROR "libsodium not found.")
    endif()
endif()

# Tạo namespace alias chuẩn cho project
add_library(deps::sodium ALIAS ${SODIUM_TARGET})
