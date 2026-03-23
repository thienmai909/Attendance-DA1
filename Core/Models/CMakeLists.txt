add_library(Models)

target_sources(Models
	PRIVATE
		DataType.cpp
		GiangVien.cpp
		SinhVien.cpp
		LopHocPhan.cpp
		BuoiDiemDanh.cpp
		DangKyHoc.cpp
	PUBLIC
		FILE_SET HEADERS
		FILES
			DataType.hpp
			GiangVien.hpp
			SinhVien.hpp
			LopHocPhan.hpp
			BuoiDiemDanh.hpp
			DangKyHoc.hpp
)

target_link_libraries(Models
	PRIVATE
		deps::sodium
		deps::nlohmann_json   
	PUBLIC
		Utils
)
