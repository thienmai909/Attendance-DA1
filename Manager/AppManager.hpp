#pragma once

#include <SinhVienManager.hpp>
#include <GiangVienManager.hpp>
#include <DangKyHocManager.hpp>

class AppManager{
    SinhVienManager _svManager;
    GiangVienManager _gvManager;
    DangKyHocManager _dkhManager;
public:
    AppManager() = default;

    void khoiDong();
    void dongLai();

    SinhVienManager& getSVManager();
    GiangVienManager& getGVManager();
    DangKyHocManager& getDKHManager();
};