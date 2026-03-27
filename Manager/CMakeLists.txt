add_library(Manager)

target_sources(Manager
    PRIVATE
        AppManager.cpp
        SinhVienManager.cpp
        GiangVienManager.cpp
        DangKyHocManager.cpp
    PUBLIC 
        FILE_SET HEADERS
    FILES
        AppManager.hpp
        SinhVienManager.hpp
        GiangVienManager.hpp
        DangKyHocManager.hpp
)

target_link_libraries(Manager
    PUBLIC
        Core
)

target_include_directories(Manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)