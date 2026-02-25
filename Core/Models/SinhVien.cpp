#include <SinhVien.hpp>

#include <format>

SinhVien::SinhVien(std::string maSV, std::string tenSV)
    : _maSV(maSV), _tenSV(tenSV)
{
}

void SinhVien::setLopSH(LopSinhHoat lopSH)
{
    _lopSinhHoat = lopSH;
}

void SinhVien::setNgaySinh(int ngay, int thang, int nam)
{
    _ngaySinh.emplace(ngay, thang, nam);
}

void SinhVien::setNgaySinh(const DateTime& ngaysinh)
{
    _ngaySinh = ngaysinh;
}

std::string SinhVien::getNgaySinhStr() const
{
    if (_ngaySinh.has_value()) {
        return std::format("{:02d}/{:02d}/{}",
                _ngaySinh->day(),
                _ngaySinh->month(),
                _ngaySinh->year());
    }

    return "(none)";
}
