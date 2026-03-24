#pragma once

#include <SinhVienManager.hpp>

class AppManager{
    SinhVienManager _svManager;

public:
    AppManager() = default;

    void khoiDong();
    void dongLai();

    SinhVienManager& getSVManager();
};