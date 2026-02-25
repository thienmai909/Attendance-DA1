#include <GiangVien.hpp>
#include <format>

GiangVien::GiangVien(std::string maGV, std::string hoTen, bool isAdmin) 
 : _maGv(std::move(maGV)), _hoTen(std::move(hoTen)), _isAdmin(isAdmin)
{
}

void GiangVien::setHoTen(std::string hoTen)
{
    _hoTen = std::move(hoTen);
}

void GiangVien::setTaiKhoan(std::string tenTaiKhoan, std::string matKhau)
{
    _taiKhoan.emplace(std::move(tenTaiKhoan), std::move(matKhau));
}

void GiangVien::setHocVi(Degree hocVi)
{
    _hocVi = hocVi;
}

void GiangVien::setLienHe(std::string email, std::string sdt)
{
    _lienHe.emplace(std::move(email), std::move(sdt));
}

std::string GiangVien::getMaGV() const
{
    return _maGv;
}

std::string GiangVien::getHoTenGV() const
{
    return _hoTen;
}

std::string GiangVien::getTenTaiKhoan() const
{
    return _taiKhoan.has_value() ? _taiKhoan->getUsername() : "(none)";
}

std::string GiangVien::getMatKhauHash() const
{
    return _taiKhoan.has_value() ? _taiKhoan->getPasswordHash() : "(none)";
}

Degree GiangVien::getHocVi() const
{
    return _hocVi;
}

std::string GiangVien::getHocViStr() const
{
    switch(_hocVi) {
        case Degree::CUNHAN:
            return "Cử nhân";
        case Degree::THACSI:
            return "Thạc sĩ";
        case Degree::TIENSI:
            return "Tiến sĩ";
        case Degree::PGSTS:
            return "PGS.TS";
        case Degree::GS:
            return "Giáo sư";
        case Degree::NONE:
            return "(None)";
    }
    return "(None)";
}

std::optional<Contact> GiangVien::getLienHe() const
{
    return _lienHe;
}

std::string GiangVien::getLienHeStr() const
{
    if (_lienHe.has_value())
        return std::format("Email: {} | Sđt: {}", 
            _lienHe->getEmail(), 
            _lienHe->getPhoneNumber()
        );
    return "";
}

bool GiangVien::xacThucTaiKhoan(const std::string &username, const std::string &password) const
{
    if (_taiKhoan.has_value()) {
        if (_taiKhoan->getUsername() == username &&
            _taiKhoan->verifyPassword(password)
        )
            return true;
    }
    return false;
}

void GiangVien::khoiPhucTaiKhoan(std::string username, std::string hash)
{
    _taiKhoan = Account::loadFromHash(std::move(username), std::move(hash));
}

utility_csv::Row GiangVien::toCSVRow() const
{
    // Chuyển enum class thành string
    std::string hocVi {};
    switch(_hocVi) {
        case Degree::CUNHAN:
            hocVi = "CUNHAN"; break;
        case Degree::THACSI:
            hocVi = "THACSI"; break;
        case Degree::TIENSI:
            hocVi = "TIENSI"; break;
        case Degree::PGSTS:
            hocVi = "PGSTS"; break;
        case Degree::GS:
            hocVi = "GS"; break;
        case Degree::NONE:
            hocVi = "NONE";
    }

    utility_csv::Row row;
    row.push_back(_maGv);
    row.push_back(_hoTen);
    row.push_back(hocVi);
    row.push_back(getTenTaiKhoan());
    row.push_back(getMatKhauHash()); //MatKhau Hash
    row.push_back(_lienHe.has_value() ? _lienHe->getEmail() : "(none)");
    row.push_back(_lienHe.has_value() ? _lienHe->getPhoneNumber() : "(none)");
    row.push_back(_isAdmin ? "1" : "0");

    return row;
}

GiangVien GiangVien::fromCSVRow(const utility_csv::Row &row)
{
    // 0 - ma; 1 - hoten; 2 - hocvi; 3 - username; 4 - hashpass; 5 email; 6 sdt; 7 - isAdmin
    GiangVien gv(row[0], row[1], row[7] == "1");

    if (!row[3].empty() && !row[4].empty())
        gv.khoiPhucTaiKhoan(row[3], row[4]);

    if (!row[5].empty() && !row[6].empty())
        gv.setLienHe(row[5], row[6]);
    
    Degree hocVi = Degree::NONE;
    if (row[2] == "CUNHAN") hocVi = Degree::CUNHAN;
    else if (row[2] == "THACSI") hocVi = Degree::THACSI;
    else if (row[2] == "TIENSI") hocVi = Degree::TIENSI;
    else if (row[2] == "PGSTS") hocVi = Degree::PGSTS;
    else if (row[2] == "GS") hocVi = Degree::GS;
    else hocVi = Degree::NONE;

    gv.setHocVi(hocVi);

    return gv;
}
