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

# Find nlohmann-json
find_package(nlohmann_json REQUIRED)

# Create a standard alias for the project
add_library(deps::nlohmann_json ALIAS nlohmann_json::nlohmann_json)

# Find csv-parser
find_package(unofficial-vincentlaucsb-csv-parser CONFIG REQUIRED)

# Create a standard alias for the project
add_library(deps::csv-parser ALIAS unofficial::vincentlaucsb-csv-parser::csv)

find_package(ftxui CONFIG REQUIRED)
add_library(deps::ftxui INTERFACE IMPORTED GLOBAL)
target_link_libraries(deps::ftxui INTERFACE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# Find libxlsxwriter (no CMake config — locate manually)
find_path(XLSXWRITER_INCLUDE_DIR NAMES xlsxwriter.h)
find_library(XLSXWRITER_LIB_RELEASE NAMES xlsxwriter CONFIGS Release)
find_library(XLSXWRITER_LIB_DEBUG   NAMES xlsxwriter CONFIGS Debug)

if(NOT XLSXWRITER_INCLUDE_DIR OR NOT XLSXWRITER_LIB_RELEASE)
    message(FATAL_ERROR "libxlsxwriter not found. Run: vcpkg install libxlsxwriter")
endif()

add_library(xlsxwriter::xlsxwriter SHARED IMPORTED GLOBAL)
set_target_properties(xlsxwriter::xlsxwriter PROPERTIES
    IMPORTED_IMPLIB         "${XLSXWRITER_LIB_RELEASE}"
    IMPORTED_IMPLIB_DEBUG   "${XLSXWRITER_LIB_DEBUG}"
    INTERFACE_INCLUDE_DIRECTORIES "${XLSXWRITER_INCLUDE_DIR}"
)
add_library(deps::xlsxwriter ALIAS xlsxwriter::xlsxwriter)