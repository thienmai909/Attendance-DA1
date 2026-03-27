#pragma once

#include <GiangVien.hpp>
#include <Utility.hpp>

#include <vector>
#include <string>
#include <optional>
#include <filesystem>

const std::string data_root = "data/";

class GiangVienManager {
    std::vector<GiangVien> _dsGiangVien;
    std::filesystem::path _filePath;
    bool _isDirty = false;

public:
    explicit GiangVienManager(std::filesystem::path filePath = std::format("{}giangvien.json", data_root));

    void load();
    void save();
    void saveIfDirty();

    void them(const GiangVien& giangVien);
    void capNhat(const GiangVien& giangVien);
    void xoa(const std::string& maGV);

    std::optional<GiangVien> timTheoMa(const std::string& maGV) const;
    std::optional<GiangVien> timTheoTaiKhoan(const std::string& username) const;
    std::vector<GiangVien> timTheoTen(const std::string& keyword) const;

    std::optional<GiangVien> dangNhap(
        const std::string& username,
        const std::string& password
    ) const;
    const std::vector<GiangVien>& getAll() const;
    std::size_t soLuong() const;
    bool isDirty() const;
private:
    std::vector<GiangVien>::iterator timIterator(const std::string& maGV);
};