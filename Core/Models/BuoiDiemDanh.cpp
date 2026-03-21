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

std::optional<DateTime> ChiTietDiemDanh::getGioDiemDanh() const
{
    return _gioDiemDanh;
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

nlohmann::json ChiTietDiemDanh::toJson() const
 {
    using namespace utility_json;
    nlohmann::json j = {
        {"maSV", _maSV},
        {"trangThai", static_cast<int>(_trangThai)},
        {"ghiChu", _ghiChu}
    };

    if (_gioDiemDanh.has_value()) 
        j["gioDiemDanh"] = _gioDiemDanh->toTimeString();
    else
        j["gioDiemDanh"] = nullptr;
    return j;
}

ChiTietDiemDanh ChiTietDiemDanh::fromJson(const nlohmann::json &j)
{
    using namespace utility_json;
    auto maSV = require<std::string>(j, "maSV");
    auto trangThai = static_cast<Status>(optional<int>(j, "trangThai", 0));
    auto ghiChu = optional<std::string>(j, "ghiChu", "");

    DateTime gio = DateTime();
    if (!j["gioDiemDanh"].is_null()) {
        std::string gioStr = require<std::string>(j, "gioDiemDanh");
        int h = 0, m = 0, s = 0;
        if (std::sscanf(gioStr.c_str(), "%d:%d:%d", &h, &m, &s) == 3) {
            auto now = DateTime();
            gio = DateTime(now.day(), now.month(), now.year(), h, m, s);
        }
    }
    return ChiTietDiemDanh(maSV, gio, trangThai, ghiChu);
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

nlohmann::json BuoiDiemDanh::toJson() const
{
    using namespace utility_json;
    nlohmann::json j = {
        {"caDiemDanh", static_cast<int>(_caDiemDanh)},
        {"soTiet", _soTiet},
        {"khoaDiemDanh", _khoaDiemDanh},
        {"chiTiet", dump_array<ChiTietDiemDanh>(
            _danhSachChiTiet,
            [](const ChiTietDiemDanh& chiTiet) { return chiTiet.toJson(); }
        )}
    };

    if (_ngayDiemDanh.has_value())
        j["ngayDiemDanh"] = _ngayDiemDanh->toDayString();
    else
        j["ngayDiemDanh"] = nullptr;
    return j;
}

BuoiDiemDanh BuoiDiemDanh::fromJson(const nlohmann::json &j)
{
    using namespace utility_json;

    DateTime ngay = DateTime();
    if (!j["ngayDiemDanh"].is_null()) {
        std::string ngayStr = require<std::string>(j, "ngayDiemDanh");
        int d = 0, m = 0, y = 0;
        if (std::sscanf(ngayStr.c_str(), "%d/%d/%d", &d, &m, &y) == 3)
            ngay = DateTime(d, m, y);
    }

    auto ca = static_cast<CaHoc>(optional<int>(j, "caDiemDanh", 0));
    auto soTiet = optional<int>(j, "soTiet", 0);
    
    BuoiDiemDanh buoi(ngay, ca, soTiet);

    buoi._danhSachChiTiet = load_array<ChiTietDiemDanh> (
        j, "chiTiet",
        [] (const nlohmann::json& item) {
            return ChiTietDiemDanh::fromJson(item);
        }
    );

    if (optional<bool>(j, "khoaDiemDanh", false))
        buoi.khoaBuoi();

    return buoi;
}
