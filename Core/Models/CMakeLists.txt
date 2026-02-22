add_library(Models)

target_sources(Models
	PRIVATE
		models.cpp
	PUBLIC
		FILE_SET HEADERS
		FILES
			models.hpp
)

target_link_libraries(Models
	PUBLIC
		deps::sodium
)
