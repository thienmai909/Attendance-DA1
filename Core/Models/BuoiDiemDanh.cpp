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

std::string ChiTietDiemDanh::getTrangThaiStr() const
{
    Status trangThai;
    switch(_trangThai) {
        case Status::CO_MAT: return "Có mặt";
        case Status::MUON: return "Muộn";
        case Status::VANG: return "Vắng";
        default: return "Default";
    }
    return "(none)";
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

BuoiDiemDanh::BuoiDiemDanh(
    DateTime ngayDiemDanh,
    CaHoc caDiemDanh,
    int soTiet
) : 
    _ngayDiemDanh(std::move(ngayDiemDanh)),
    _caDiemDanh(caDiemDanh),
    _soTiet(soTiet)
{
}

std::string BuoiDiemDanh::getNgayDiemDanhStr() const
{
    if (_ngayDiemDanh.has_value())
        return _ngayDiemDanh->toDayString();
    return "(none)";
}

std::optional<DateTime> BuoiDiemDanh::getNgayDiemDanh() const
{
    return _ngayDiemDanh;
}

CaHoc BuoiDiemDanh::getCaDiemDanh() const
{
    return _caDiemDanh;
}

std::string BuoiDiemDanh::getCaDiemDanhStr() const
{
    CaHoc caDiemDanh;
    switch(_caDiemDanh) {
        case CaHoc::SANG: return "SÁNG";
        case CaHoc::CHIEU: return "CHIỀU";
        case CaHoc::TOI: return "TỐI";
        default: return "DEFAULT";
    }
    return "(none)";
}

int BuoiDiemDanh::getSoTiet() const
{
    return _soTiet;
}

bool BuoiDiemDanh::isKhoaDiemDanh() const
{
    return _khoaDiemDanh;
}

const std::vector<ChiTietDiemDanh> &BuoiDiemDanh::getDanhSachChiTiet() const
{
    return _danhSachChiTiet;
}

void BuoiDiemDanh::khoaBuoi()
{
    _khoaDiemDanh = true;
}

void BuoiDiemDanh::themChiTiet(
    const std::string &maSV,
    const DateTime &gioDiemDanh,
    Status trangThai,
    const std::string &ghiChu
)
{
    if (_khoaDiemDanh)
        throw std::runtime_error("Buổi điểm danh đã bị khóa!");
    
    for (const auto& chiTiet : _danhSachChiTiet)
        if (chiTiet.getMaSV() == maSV)
            throw std::invalid_argument("Sinh viên đã được điểm danh!");
    _danhSachChiTiet.emplace_back(maSV, gioDiemDanh, trangThai, ghiChu);
}

void BuoiDiemDanh::capNhatTrangThai(const std::string &maSV, Status trangThai)
{
    if (_khoaDiemDanh)
        throw std::runtime_error("Buổi điểm danh đã bị khóa!");

    for (auto& chiTiet : _danhSachChiTiet)
        if (chiTiet.getMaSV() == maSV) {
            chiTiet.setTrangThai(trangThai);
            return;
        }
    throw std::invalid_argument("Không tìm thấy sinh viên!");
}

void BuoiDiemDanh::capNhatGhiChu(const std::string &maSV, const std::string &ghiChu)
{
        if (_khoaDiemDanh)
        throw std::runtime_error("Buổi điểm danh đã bị khóa!");

    for (auto& chiTiet : _danhSachChiTiet)
        if (chiTiet.getMaSV() == maSV) {
            chiTiet.setGhiChu(ghiChu);
            return;
        }
    throw std::invalid_argument("Không tìm thấy sinh viên!");
}

int BuoiDiemDanh::demVangMat() const
{
    int count = 0;
    for (const auto& chiTiet : _danhSachChiTiet)
        if (chiTiet.getTrangThai() == Status::VANG) ++count;
    return count;
}
