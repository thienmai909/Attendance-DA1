#include <LopHocPhan.hpp>

LopHocPhan::LopHocPhan(
    std::string maLHP, 
    std::string tenLHP, 
    int soTC, 
    int tongSoTiet, 
    double nguongCamThi, 
    HocKi hocKi
) :
    _maLHP(std::move(maLHP)), 
    _tenLHP(std::move(tenLHP)), 
    _soTC(soTC), 
    _tongSoTiet(tongSoTiet), 
    _nguongCamThi(nguongCamThi),
    _hocKi(hocKi)
{
}

const std::string &LopHocPhan::getMaLHP() const
{
    return _maLHP;
}

const std::string &LopHocPhan::getTenLHP() const
{
    return _tenLHP;
}

int LopHocPhan::getSoTC() const
{
    return _soTC;
}

int LopHocPhan::getTongSoTiet() const
{
    return _tongSoTiet;
}

int LopHocPhan::getSoTietDaHoc() const
{
    return _soTietDaHoc;
}

int LopHocPhan::getSoBuoiDaHoc() const
{
    return _soBuoiDaHoc;
}

double LopHocPhan::getNguongCamThi() const
{
    return _nguongCamThi;
}

HocKi LopHocPhan::getHocKi() const
{
    return _hocKi;
}

std::optional<ClassRoom> LopHocPhan::getPhongHoc() const
{
    return _phongHoc;
}

void LopHocPhan::setTenLHP(const std::string &tenLHP)
{
    _tenLHP = tenLHP;
}

void LopHocPhan::setSoTC(int soTC)
{
    _soTC = soTC;
}

void LopHocPhan::setNguongCamThi(double nguongCamThi)
{
    _nguongCamThi = nguongCamThi;
}

void LopHocPhan::setPhongHoc(const ClassRoom &phongHoc)
{
    _phongHoc = phongHoc;
}

int LopHocPhan::soTietVangToiDa() const
{
    return static_cast<int>(_tongSoTiet * _nguongCamThi);
}

bool LopHocPhan::kiemTraViPham(int soTietVang) const
{
    return soTietVang > this->soTietVangToiDa();
}

void LopHocPhan::ghiNhanBuoiHoc(int soTiet)
{
    _soBuoiDaHoc += 1;
    _soTietDaHoc += soTiet;
}

double LopHocPhan::tiLeVangHienTai(int tongSoTietVang) const
{
    return tongSoTietVang / _soTietDaHoc;
}

int LopHocPhan::soTietVangDoiDaChoPhep() const
{
    return _tongSoTiet * _nguongCamThi;
}

std::string LopHocPhan::tienDoHocTapStr() const
{
    return std::format("Đã học: {} buổi ({}/{} tiết) - {:.2f}%", 
        this->getSoBuoiDaHoc(),
        this->getSoTietDaHoc(),
        this->getTongSoTiet(),
        static_cast<double>(this->getSoTietDaHoc()) / this->getTongSoTiet() * 100.0
    );
}

utility_csv::Row LopHocPhan::toCSVRow() const
{
    std::string hocKi{};
    switch(_hocKi) {
        case HocKi::I: 
            hocKi = "I"; break;
        case HocKi::II:
            hocKi = "II"; break;
        case HocKi::He:
            hocKi = "He"; break;
        default:
            hocKi = "DEFAULT";
    }

    utility_csv::Row row;
    row.push_back(_maLHP);
    row.push_back(_tenLHP);
    row.push_back(std::to_string(_soTC));
    row.push_back(std::to_string(_tongSoTiet));
    row.push_back(std::to_string(_soTietDaHoc));
    row.push_back(std::to_string(_soBuoiDaHoc));
    row.push_back(std::to_string(_nguongCamThi));
    row.push_back(hocKi);
    row.push_back(_phongHoc.has_value() ? _phongHoc->getLoaiPhong() : "(none)");
    row.push_back(_phongHoc.has_value() ? _phongHoc->getTenPhong() : "(none)");
    row.push_back(_phongHoc.has_value() ? std::to_string(_phongHoc->getSucChua()) : "(none)");
    
    return row;
}
