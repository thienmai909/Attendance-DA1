add_library(Models)

target_sources(Models
	PRIVATE
		DataType.cpp
		GiangVien.cpp
		SinhVien.cpp
		LopHocPhan.cpp
	PUBLIC
		FILE_SET HEADERS
		FILES
			DataType.hpp
			GiangVien.hpp
			SinhVien.hpp
			LopHocPhan.hpp
)

target_link_libraries(Models
	PRIVATE
		deps::sodium
	PUBLIC
		Utils
)
