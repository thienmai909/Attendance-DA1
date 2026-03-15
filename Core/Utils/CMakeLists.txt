add_library(Utils INTERFACE)

target_sources(Utils
	INTERFACE
		FILE_SET HEADERS
		FILES
			Utility.hpp
)

target_include_directories(Utils
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Utils 
	INTERFACE
		deps::csv-parser
		deps::nlohmann_json
)

