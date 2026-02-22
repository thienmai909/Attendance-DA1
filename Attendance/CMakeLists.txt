add_executable(Attendance)
target_sources(Attendance
	PRIVATE
		Attendance.cxx
)

target_compile_features(Attendance
	PRIVATE
		cxx_std_20
)

if (
	(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR
	(CMAKE_CXX_COMPILER_FRONTED_VARIANT STREQUAL "MSVC")
) 
	target_compile_options(Attendance
		PRIVATE
			/W3
	)
elseif (
	(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
	(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
) 
	target_compile_options(Attendance
		PRIVATE
			-Wall
	)
endif()

target_link_libraries(Attendance
	PUBLIC
		Core
)
