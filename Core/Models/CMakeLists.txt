add_library(Models)

target_sources(Models
	PRIVATE
		DataType.cpp
	PUBLIC
		FILE_SET HEADERS
		FILES
			DataType.hpp
)

target_link_libraries(Models
	PRIVATE
		deps::sodium
)
