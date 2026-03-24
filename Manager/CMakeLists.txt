add_library(Manager)

target_sources(Manager
    PRIVATE
        AppManager.cpp
        SinhVienManager.cpp
    PUBLIC 
        FILE_SET HEADERS
    FILES
        AppManager.hpp
        SinhVienManager.hpp
)

target_link_libraries(Manager
    PUBLIC
        Core
)

target_include_directories(Manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)