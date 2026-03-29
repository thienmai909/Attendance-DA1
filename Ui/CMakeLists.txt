add_library(Ui)

target_sources(Ui
    PRIVATE
        UiManager.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES
            UiManager.hpp
)

target_link_libraries(Ui
    PUBLIC
        Manager
        deps::ftxui
)

target_include_directories(Ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Screen
)