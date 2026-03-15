#include <BuoiDiemDanh.hpp>

ChiTietDiemDanh::ChiTietDiemDanh(
    std::string maSV,
    DateTime gioDiemDanh,
    Status trangThai,
    std::string ghiChu
) : _maSV(std::move(maSV)), 
    _gioDiemDanh(std::move(gioDiemDanh)), 
    _trangThai(trangThai), 
    _ghiChu(std::move(ghiChu))
{
}

const std::string &ChiTietDiemDanh::getMaSV() const
{
    return _maSV;
}

Status ChiTietDiemDanh::getTrangThai() const
{
    return _trangThai;
}

const std::string &ChiTietDiemDanh::getGhiChu() const
{
    return _ghiChu;
}

std::string ChiTietDiemDanh::getGioDiemDanhStr() const
{
    return _gioDiemDanh.has_value() ? _gioDiemDanh->toTimeString() : "(none)";
}

void ChiTietDiemDanh::setTrangThai(Status trangThai)
{
    _trangThai = trangThai;
}

void ChiTietDiemDanh::setGhiChu(const std::string &ghiChu)
{
    _ghiChu = ghiChu;
}


// ================ BuoiDiemDanh ================