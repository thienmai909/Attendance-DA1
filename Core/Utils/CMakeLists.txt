add_library(Utils INTERFACE)

target_sources(Utils
	INTERFACE
		FILE_SET HEADERS
		FILES
			Utility.hpp
)
