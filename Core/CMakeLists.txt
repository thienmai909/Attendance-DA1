add_library(Core INTERFACE)

target_link_libraries(Core
	INTERFACE
		Models
		Utils
)

add_subdirectory(Models)
add_subdirectory(Utils)
