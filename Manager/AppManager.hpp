#pragma once

#include <SinhVienManager.hpp>
#include <GiangVienManager.hpp>
#include <DangKyHocManager.hpp>
#include <LopHocPhanManager.hpp>
#include <DiemDanhManager.hpp>

class AppManager{
    SinhVienManager _svManager;
    GiangVienManager _gvManager;
    LopHocPhanManager _lhpManager;
    DangKyHocManager _dkhManager;
    DiemDanhManager _ddManager;
public:
    AppManager();

    void khoiDong();
    void dongLai();

    SinhVienManager& getSVManager();
    GiangVienManager& getGVManager();
    DangKyHocManager& getDKHManager();
    LopHocPhanManager& getLHPManager();
    DiemDanhManager& getDDManager();
};