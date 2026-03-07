#pragma once

#include <DataType.hpp>

#include <optional>
#include <string>

class LopHocPhan {
    std::string _maLHP;
    std::string _tenLHP;
    int _soTC;
    int _tongSoTiet;
    double _nguongCamThi;
    HocKi _hocKi = HocKi::DEFAULT;
    std::optional<ClassRoom> _phongHoc;
public:
    LopHocPhan() = default;
    LopHocPhan(
        std::string maLHP,
        std::string tenLHP,
        int soTC, int tongSoTiet,
        double nguongCamThi,
        HocKi hocKi
    );
};