#pragma once

#include <DataType.hpp>
#include <Utility.hpp>

#include <string>
#include <optional>
#include <vector>

class DangKyHoc {
    std::string _maSV;
    std::string _maLopHocPhan;
    DateTime _ngayDangKy;
    bool _isActive = true;

public:
    DangKyHoc(
        std::string maSV,
        std::string maLopHocPhan,
        const DateTime& ngayDangKy,
        bool isActive
    );

    const std::string& getMaSV() const;
    const std::string& getMaLopHocPhan() const;
    DateTime getNgayDangKy() const;
    std::string getNgayDangKyStr() const;
    bool isActive() const;

    void huyDangKy();
    void kichHoatLai();
    bool trungVoi(const std::string& maSV, const std::string& maLopHocPhan) const;
    
    bool operator<(const DangKyHoc& other) const;
    bool operator==(const DangKyHoc& other) const;

    nlohmann::json toJson() const;
    static DangKyHoc fromJson(const nlohmann::json& j);
};