#include <SinhVien.hpp>

#include <format>

std::string SinhVien::lopSinhHoatStr(LopSinhHoat lopSinhHoat)
{
    switch(lop) {
        case LopSinhHoat::DHSTIN23A: return "DHSTIN23A";
        case LopSinhHoat::DHSTIN23B: return "DHSTIN23AB;
        case LopSinhHoat::DHSTIN23C: return "DHSTIN23AC;
        default:                     return "DEFAULT";
    };
    return "(none)";
}

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

void SinhVien::setLienHe(const std::string &email, const std::string &sdt)
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

const std::string& SinhVien::getMaSV() const
{
    return _maSV;
}

const std::string& SinhVien::getTenSV() const
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

bool SinhVien::isValid() const
{
    return !_maSV.empty() && !_tenSV.empty();
}

bool SinhVien::hasValidMaSV() const
{
    if (_maSV.size() != 10) return false;
    return std::all_of(_maSV.begin(), _maSV.end(), std::isdigit);
}

bool SinhVien::hasLienHe() const
{
    return _lienHe.has_value();
}

bool SinhVien::hasNgaySinh() const
{
    return _ngaySinh.has_value();
}

std::optional<int> SinhVien::tinhTuoi() const
{
    if (!_ngaySinh.has_value()) return std::nullopt;
    DateTime now = DateTime();
    int tuoi = now.year() - _ngaySinh->year();
    if (now.month() < _ngaySinh->month() ||
        (now.month() == _ngaySinh->month() && 
         now.day() < _ngaySinh->day())
    )
        --tuoi;
    return tuoi;
}

bool SinhVien::isSinhNhat() const
{
    if (!_ngaySinh.has_value()) return false;
    DateTime now = DateTime();
    return now.day() == _ngaySinh->day() &&
           now.month() == _ngaySinh->month();
}

bool SinhVien::operator==(const SinhVien &other) const
{
    return _maSV == other._maSV;
}

bool SinhVien::operator<(const SinhVien &other) const
{
    return _maSV < other._maSV;
}

bool SinhVien::matchTen(const std::string &keyword) const
{
    std::string tenLower = _tenSV;
    std::string kwLower = keyword;
    std::transform(tenLower.begin(), tenLower.end(), tenLower.begin(), std::tolower);
    std::transform(kwLower.begin(), kwLower.end(), kwLower.begin(), std::tolower);
    return tenLower.find(kwLower) != std::string::npos;
}

std::string SinhVien::toSummaryString() const
{
    return _maSV + " -  " + _tenSV + " (" + lopSinhHoatStr(_lopSinhHoat) + ")";
}

std::string SinhVien::toDetailString() const
{
    std::ostringstream oss;
    oss << "Mã SV     : " << _maSV << "\n"
        << "Họ tên    : " << _tenSV << "\n"
        << "Lớp SH    : " << lopSinhHoatStr(_lopSinhHoat) << "\n"
        << "Ngày sinh : " << getNgaySinhStr() << "\n"
        << "Liên hệ   : " << getLienHeStr();
    if (auto tuoi = tinhTuoi)
        oss << "\nTuổi      : " << *tuoi;
    return oss.str();
}

utility_csv::Row SinhVien::toCSVRow() const
{
    std::string lopSH {};
    switch(_lopSinhHoat) {
        case LopSinhHoat::DHSTIN23A:
            lopSH = "DHSTIN23A"; break;
        case LopSinhHoat::DHSTIN23B:
            lopSH = "DHSTIN23B"; break;
        case LopSinhHoat::DHSTIN23C:
            lopSH = "DHSTIN23C"; break;
        case LopSinhHoat::DEFAULT:
            lopSH = "DEFAULT";
    }

    utility_csv::Row row;
    row.push_back(_maSV);
    row.push_back(_tenSV);
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->day()) : "(none)");
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->month()) : "(none)");
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->year()) : "(none)");
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->hour()) : "(none)");
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->minute()) : "(none)");
    row.push_back(_ngaySinh.has_value() ? std::to_string(_ngaySinh->second()) : "(none)");
    row.push_back(_lienHe.has_value() ? _lienHe->getEmail() : "(none)");
    row.push_back(_lienHe.has_value() ? _lienHe->getPhoneNumber() : "(none)");
    row.push_back(lopSH);

    return row;
}

SinhVien SinhVien::fromCSVRow(const utility_csv::Row &row)
{
    SinhVien sv(row[0], row[1]);

    if (row[2] != "(none)" && !row[2].empty() && 
        row[3] != "(none)" && !row[3].empty() && 
        row[4] != "(none)" && !row[4].empty() && 
        row[5] != "(none)" && !row[5].empty() && 
        row[6] != "(none)" && !row[6].empty() && 
        row[7] != "(none)" && !row[7].empty()
    )
        sv.setNgaySinh(
            DateTime(std::stoi(row[2]), 
                    std::stoi(row[3]), 
                    std::stoi(row[4]),
                    std::stoi(row[5]), 
                    std::stoi(row[6]),
                    std::stoi(row[7])
            )
        );

    if (!row[8].empty() && row[8] != "(none)" && 
        !row[9].empty() && row[9] != "(none)"
    )
        sv.setLienHe(row[8], row[9]);

    LopSinhHoat lopSH = LopSinhHoat::DEFAULT;
    if (row[10] == "DHSTIN23A") lopSH = LopSinhHoat::DHSTIN23A;
    else if (row[10] == "DHSTIN23B") lopSH = LopSinhHoat::DHSTIN23B;
    else if (row[10] == "DHSTIN23C") lopSH = LopSinhHoat::DHSTIN23C;
    else lopSH = LopSinhHoat::DEFAULT;

    sv.setLopSH(lopSH);

    return sv;
}
