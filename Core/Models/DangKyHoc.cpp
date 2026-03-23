#include <DangKyHoc.hpp>

DangKyHoc::DangKyHoc(
    std::string maSV,
    std::string maLopHocPhan,
    const DateTime &ngayDangKy,
    bool isActive
) : 
    _maSV(std::move(maSV)),
    _maLopHocPhan(std::move(maLopHocPhan)),
    _ngayDangKy(ngayDangKy),
    _isActive(isActive)
{}

const std::string &DangKyHoc::getMaSV() const
{
    return _maSV;
}

const std::string &DangKyHoc::getMaLopHocPhan() const
{
    return _maLopHocPhan;
}

DateTime DangKyHoc::getNgayDangKy() const
{
    return _ngayDangKy;
}

std::string DangKyHoc::getNgayDangKyStr() const
{
    return _ngayDangKy.toDayString();
}

bool DangKyHoc::isActive() const
{
    return _isActive;
}

void DangKyHoc::huyDangKy()
{
    if (!_isActive) 
        throw std::runtime_error("Đăng ký bị hủy trước đó!");
    _isActive = false;
}

void DangKyHoc::kichHoatLai()
{
    if (_isActive)
        throw std::runtime_error("Đăng ký đang hoạt động!");
    _isActive = true;
}

bool DangKyHoc::trungVoi(const std::string &maSV, const std::string &maLopHocPhan) const
{
    return _maSV == maSV && _maLopHocPhan == maLopHocPhan;
}

bool DangKyHoc::operator<(const DangKyHoc &other) const
{
    return _ngayDangKy < other._ngayDangKy;
}

bool DangKyHoc::operator==(const DangKyHoc &other) const
{
    return _maSV == other._maSV && _maLopHocPhan == other._maLopHocPhan;
}

nlohmann::json DangKyHoc::toJson() const
{
    return {
        {"maSV", _maSV},
        {"maLopHocPhan", _maLopHocPhan},
        {"ngayDangKy", _ngayDangKy.toDayString()},
        {"isActive", _isActive}
    };
}

DangKyHoc DangKyHoc::fromJson(const nlohmann::json &j)
{
    using namespace utility_json;

    auto maSV = require<std::string>(j, "maSV");
    auto maLopHocPhan = require<std::string>(j, "maLopHocPhan");
    auto ngayStr = require<std::string>(j, "ngayDangKy");
    auto active = optional<bool>(j, "isActive", true);

    int d = 0, m = 0, y = 0;
    if (std::sscanf(ngayStr.c_str(), "%d/%d/%d", &d, &m, &y) != 3)
        throw std::runtime_error("DangKyHoc: ngayDangKy sai định dạng: " + ngayStr);
    
    DangKyHoc dangKyHoc(maSV, maLopHocPhan, DateTime(d, m, y), active);
    return dangKyHoc;
}
