#pragma once

#include <SinhVienManager.hpp>
#include <GiangVienManager.hpp>
#include <DangKyHocManager.hpp>
#include <LopHocPhanManager.hpp>
#include <DiemDanhManager.hpp>
#include <ThongKeManager.hpp>

class AppManager{
    SinhVienManager _svManager;
    GiangVienManager _gvManager;
    LopHocPhanManager _lhpManager;
    DangKyHocManager _dkManager;
    DiemDanhManager _ddManager;
    ThongKeManager _tkManager;
public:
    AppManager();

    void khoiDong();
    void dongLai();

    SinhVienManager& getSVManager();
    GiangVienManager& getGVManager();
    DangKyHocManager& getDKManager();
    LopHocPhanManager& getLHPManager();
    ThongKeManager& getTKManager();
};