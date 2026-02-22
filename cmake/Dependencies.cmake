# cmake/Dependencies.cmake

set(SODIUM_TARGET "")

if(WIN32)
	find_package(unofficial-sodium CONFIG QUIET)
    	set(SODIUM_TARGET unofficial-sodium::sodium)
else()
	find_package(PkgConfig REQUIRED)
    	pkg_check_modules(SODIUM REQUIRED libsodium)

    	add_library(sodium_external INTERFACE)
    	target_include_directories(sodium_external INTERFACE ${SODIUM_INCLUDE_DIRS})
    	target_link_libraries(sodium_external INTERFACE ${SODIUM_LINK_LIBRARIES})

    	set(SODIUM_TARGET sodium_external)
	#find_package(sodium REQUIRED)
	#set(SODIUM_TARGET sodium)
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
