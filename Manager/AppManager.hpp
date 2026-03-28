#pragma once

#include <SinhVienManager.hpp>
#include <GiangVienManager.hpp>
#include <DangKyHocManager.hpp>
#include <LopHocPhanManager.hpp>

class AppManager{
    SinhVienManager _svManager;
    GiangVienManager _gvManager;
    DangKyHocManager _dkhManager;
    LopHocPhanManager _lhpManager;
public:
    AppManager() = default;

    void khoiDong();
    void dongLai();

    SinhVienManager& getSVManager();
    GiangVienManager& getGVManager();
    DangKyHocManager& getDKHManager();
    LopHocPhanManager& getLHPManager();
};