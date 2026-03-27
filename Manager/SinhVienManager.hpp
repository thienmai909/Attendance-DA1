#pragma once

#include <SinhVien.hpp>
#include <Utility.hpp>

#include <vector>
#include <string>
#include <format>
#include <optional>
#include <algorithm>
#include <filesystem>

class SinhVienManager {
    std::vector<SinhVien> _dsSinhVien;
    std::filesystem::path _filePath;
    bool _isDirty = false; // Đánh dấu có thay đổi chưa lưu

    std::vector<SinhVien>::iterator timIterator(const std::string& maSV);

public:
    explicit SinhVienManager(std::filesystem::path filePath = "data/sinhvien.json");

    void load();
    void save();
    void saveIfDirty();
    
    void them(const SinhVien& sinhVien);
    void capNhat(const SinhVien& sinhVien);
    void xoa(const std::string& maSV);

    std::optional<SinhVien> timTheoMa(const std::string& maSV) const;
    std::vector<SinhVien> timTheoTen(const std::string& keyword) const;
    std::vector<SinhVien> locTheoLop(LopSinhHoat lopSinhHoat) const;
    const std::vector<SinhVien>& getAll() const;
    std::size_t soLuong() const;
    bool isDirty() const;
};