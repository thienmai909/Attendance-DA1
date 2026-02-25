#pragma once

#include <DataType.hpp>

#include <optional>
#include <string>

class SinhVien {
    std::string _maSV;
    std::string _tenSV;
    std::optional<DateTime> _ngaySinh;
    std::optional<Contact> _lienHe;
    LopSinhHoat _lopSinhHoat = LopSinhHoat::DEFAULT;

public:
    SinhVien() = default;
    SinhVien(std::string maSV, std::string tenSV);

    void setLopSH(LopSinhHoat lopSH);
    void setNgaySinh(int ngay, int thang, int nam);
    void setNgaySinh(const DateTime& ngaysinh);
    void setLienHe(std::string email, std::string sdt);
    void setLienHe(const DateTime& lienHe);

    LopSinhHoat getLopSH() const;
    std::string getMaSV() const;
    std::string getTenSV() const;
    std::optional<DateTime> getNgaySinh() const;
    std::string getNgaySinhStr() const;
    std::optional<Contact> getLienHe() const;
    std::string getLienHeStr() const;
};