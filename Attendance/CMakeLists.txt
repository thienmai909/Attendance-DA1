add_executable(Attendance)
target_sources(Attendance PRIVATE Attendance.cpp)

# --- Seed: tao du lieu mau (chay 1 lan) ---
add_executable(Seed)
target_sources(Seed PRIVATE seed.cpp)

if (
	(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR
	(CMAKE_CXX_COMPILER_FRONTED_VARIANT STREQUAL "MSVC")
)
	target_compile_options(Attendance PRIVATE /W3)
	target_compile_options(Seed       PRIVATE /W3)
elseif (
	(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
	(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
)
	target_compile_options(Attendance PRIVATE -Wall)
	target_compile_options(Seed       PRIVATE -Wall)
endif()

target_link_libraries(Attendance PUBLIC Core Manager Ui)
target_link_libraries(Seed       PUBLIC Core Manager)
