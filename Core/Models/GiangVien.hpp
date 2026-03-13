#pragma once

#include <DataType.hpp>
#include <Utility.hpp>

#include <string>
#include <optional>

class GiangVien {
    std::string _maGv;
    std::string _hoTen;
    std::optional<Account> _taiKhoan;
    std::optional<Contact> _lienHe;
    Degree _hocVi = Degree::NONE;
    bool _isAdmin;
public:
    GiangVien(std::string maGV, std::string hoTen, bool isAdmin = false);
    
    void setHoTen(const std::string& hoTen);
    void setTaiKhoan(const std::string& tenTaiKhoan, const std::string& matKhau);
    void setHocVi(Degree hocVi);
    void setLienHe(const std::string& email, const std::string& sdt);

    const std::string& getMaGV() const;
    const std::string& getHoTenGV() const;
    std::string getTenTaiKhoan() const;
    std::string getMatKhauHash() const;
    Degree getHocVi() const;
    std::string getHocViStr() const;
    std::optional<Contact> getLienHe() const;
    std::string getLienHeStr() const;

    bool xacThucTaiKhoan(const std::string& username, const std::string& password) const;
    void khoiPhucTaiKhoan(const std::string& username, const std::string& hash);
    utility_csv::Row toCSVRow() const;

    static GiangVien fromCSVRow(const utility_csv::Row& row);
};