#include <LopHocPhan.hpp>

void LopHocPhan::setSoTietDaHoc(int soTietDaHoc)
{
    _soTietDaHoc = soTietDaHoc;
}

void LopHocPhan::setSoBuoiDaHoc(int soBuoiDaHoc)
{
    _soBuoiDaHoc = soBuoiDaHoc;
}

LopHocPhan::LopHocPhan(
    std::string maLHP,
    std::string tenLHP,
    int soTC,
    int tongSoTiet,
    double nguongCamThi,
    HocKi hocKi) : _maLHP(std::move(maLHP)),
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

std::string LopHocPhan::getHocKiStr() const
{
    switch(_hocKi) {
        case HocKi::I:
            return std::format("I");
        case HocKi::II:
            return std::format("II");
        case HocKi::He:
            return std::format("Hè");
    };
    return std::format("(none)");
}

std::optional<ClassRoom> LopHocPhan::getPhongHoc() const
{
    return _phongHoc;
}

std::string LopHocPhan::getTenPhongHoc() const
{
    if (_phongHoc.has_value()) {
        return std::format("{}", _phongHoc->getTenPhong());
    }
    return "(none)";
}

const std::vector<BuoiDiemDanh> &LopHocPhan::getDsBuoiDiemDanh() const
{
    return _dsBuoiDiemDanh;
}

void LopHocPhan::setTenLHP(const std::string &tenLHP)
{
    _tenLHP = tenLHP;
}

void LopHocPhan::setSoTC(int soTC)
{
    _soTC = soTC;
}

void LopHocPhan::setTongSoTiet(int tongSoTiet)
{
    _tongSoTiet = tongSoTiet;
}

void LopHocPhan::setNguongCamThi(double nguongCamThi)
{
    _nguongCamThi = nguongCamThi;
}

void LopHocPhan::setPhongHoc(const ClassRoom &phongHoc)
{
    _phongHoc = phongHoc;
}

void LopHocPhan::setPhongHoc(const std::string &tenPhong, int sucChua, RoomType loaiPhong)
{
    _phongHoc.emplace(tenPhong, sucChua, loaiPhong);
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

void LopHocPhan::themBuoiDiemDanh(const DateTime &ngayDiemDanh, CaHoc caDiemDanh, int soTiet)
{
    for (const auto& buoi : _dsBuoiDiemDanh) {
        const auto& buoi_1 = buoi.getNgayDiemDanh();
        CaHoc ca_buoi_1 = buoi.getCaDiemDanh();
        if (buoi_1.has_value())
            if (
                buoi_1->year() == ngayDiemDanh.year() &&
                buoi_1->month() == ngayDiemDanh.month() &&
                buoi_1->day() == ngayDiemDanh.day() &&
                ca_buoi_1 == caDiemDanh
            )
                throw std::runtime_error("Buổi điểm danh bị trùng");
        }
    _dsBuoiDiemDanh.emplace_back(ngayDiemDanh, caDiemDanh, soTiet);
}

BuoiDiemDanh &LopHocPhan::getBuoi(std::size_t index)
{
    if (index >= _dsBuoiDiemDanh.size())
        throw std::out_of_range("Index số buổi không hợp lệ!");
    return _dsBuoiDiemDanh.at(index);
}

std::size_t LopHocPhan::soBuoi() const
{
    return _dsBuoiDiemDanh.size();
}

int LopHocPhan::tongTietDaDiemDanh() const
{
    int tong = 0;
    for (const auto& buoi : _dsBuoiDiemDanh)
        if (buoi.isKhoaDiemDanh())
            tong += buoi.getSoTiet();
    return tong;
}

double LopHocPhan::tyLeVang(const std::string &maSV) const
{
    if (_dsBuoiDiemDanh.empty()) return 0.0;
    int vang = 0, tong = 0;
    for (const auto& buoi : _dsBuoiDiemDanh)
        for (const auto& chiTiet : buoi.getDanhSachChiTiet())
            if (chiTiet.getMaSV() == maSV) {
                ++tong;
                if (chiTiet.getTrangThai() == Status::VANG)
                    ++vang;
            }
    return tong > 0 ? (double)vang / tong : 0.0;
}

bool LopHocPhan::biCamThi(const std::string &maSV) const
{
    return tyLeVang(maSV) > _nguongCamThi;
}

bool LopHocPhan::matchTen(const std::string &keyword) const
{
    auto toLower = [] (const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); }
        );
        return result;
    };
    return toLower(_tenLHP).find(toLower(keyword)) != std::string::npos;
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

LopHocPhan LopHocPhan::fromCSVRow(const utility_csv::Row &row)
{   
    HocKi hocKi = HocKi::DEFAULT;
    if (row[7] == "I") hocKi = HocKi::I;
    else if (row[7] == "II") hocKi = HocKi::II;
    else if (row[7] == "He") hocKi = HocKi::He;
    else hocKi = HocKi::DEFAULT;

    LopHocPhan lopHocPhan(
        row[0],             //maLHP
        row[1],             //tenLHP
        std::stoi(row[2]),  //soTC
        std::stoi(row[3]),  //tongSoTiet
        std::stof(row[6]),  //nguongCamThi
        hocKi
    );

    lopHocPhan.setSoTietDaHoc(std::stoi(row[4]));
    lopHocPhan.setSoBuoiDaHoc(std::stoi(row[5]));

    RoomType loaiPhong;

    if (!row[8].empty() && row[8] != "(none)" && 
        !row[9].empty() && row[9] != "(none)" &&
        !row[10].empty() && row[10] != "(none)"
    ) {

        if (row[8] == "Phòng lý thuyết")
            loaiPhong = RoomType::PhongLyThuyet;
        else if (row[8] == "Phòng thực hành")
            loaiPhong = RoomType::PhongThucHanh;     
        
        ClassRoom phongHoc(row[9], std::stoi(row[10]), loaiPhong);
        lopHocPhan.setPhongHoc(phongHoc);
    }

    return lopHocPhan;
}
