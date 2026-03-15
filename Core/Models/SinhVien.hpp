#pragma once

#include <DataType.hpp>
#include <Utility.hpp>

#include <optional>
#include <string>

enum class LopSinhHoat {
    DEFAULT = 0, DHSTIN23B, DHSTIN23A, DHSTIN23C
};

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
    void setLienHe(const std::string& email, const std::string& sdt);
    void setLienHe(const Contact& lienHe);

    LopSinhHoat getLopSH() const;
    const std::string& getMaSV() const;
    const std::string& getTenSV() const;
    std::optional<DateTime> getNgaySinh() const;
    std::string getNgaySinhStr() const;
    std::optional<Contact> getLienHe() const;
    std::string getLienHeStr() const;

    utility_csv::Row toCSVRow() const;

    static SinhVien fromCSVRow(const utility_csv::Row& row);
};