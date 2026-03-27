#pragma once

#include <DangKyHoc.hpp>
#include <Utility.hpp>

#include <vector>
#include <string>
#include <optional>
#include <filesystem>

class DangKyHocManager {
    std::vector<DangKyHoc> _dsDangKy;
    std::filesystem::path _filePath;
    bool _isDirty = false;

public:
    explicit DangKyHocManager(std::filesystem::path filePath = "data/dangkyhoc.json");

    void load();
    void save();
    void saveIfDirty();

    void dangKy(const std::string& maSV, const std::string& maLHP);
    void huyDangKy(const std::string& maSV, const std::string& maLHP);
    void kichHoatLai(const std::string& maSV, const std::string& maLHP);
    bool daDangKy(const std::string& maSV, const std::string& maLHP) const;

    std::vector<std::string> getDsMaSVTheoLop(const std::string& maLHP) const;
    std::vector<std::string> getDsMaLHPTheoSV(const std::string& maSV) const;
    std::vector<DangKyHoc> getLichSuTheoSV(const std::string& maSV) const;
    std::vector<DangKyHoc> getLichSuTheoLop(const std::string& maLHP) const;

    const std::vector<DangKyHoc>& getAll() const;
    std::size_t soLuong() const;
    bool isDirty() const;
private:
    std::vector<DangKyHoc>::iterator timIterator(
        const std::string& maSV,
        const std::string& maLHP
    );
};