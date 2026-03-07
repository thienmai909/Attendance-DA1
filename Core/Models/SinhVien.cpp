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

void SinhVien::setLienHe(std::string email, std::string sdt)
{
    _lienHe.emplace(email, sdt);
}

void SinhVien::setLienHe(const Contact &lienHe)
{
    _lienHe.emplace(lienHe.getEmail(), lienHe.getPhoneNumber());
}

LopSinhHoat SinhVien::getLopSH() const
{
    return _lopSinhHoat;
}

std::string SinhVien::getMaSV() const
{
    return _maSV;
}

std::string SinhVien::getTenSV() const
{
    return _tenSV;
}

std::optional<DateTime> SinhVien::getNgaySinh() const
{
    return _ngaySinh;
}

std::string SinhVien::getNgaySinhStr() const
{
    if (_ngaySinh.has_value())
        return std::format("{:02d}/{:02d}/{}",
                _ngaySinh->day(),
                _ngaySinh->month(),
                _ngaySinh->year()
        );

    return "(none)";
}

std::optional<Contact> SinhVien::getLienHe() const
{
    return _lienHe;
}

std::string SinhVien::getLienHeStr() const
{
    if (_lienHe.has_value()) 
        return std::format("Email: {} | Sđt: {}",
            _lienHe->getEmail(),
            _lienHe->getPhoneNumber()
        );
    return "(none)";
}
