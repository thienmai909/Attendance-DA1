#pragma once
#include <DataType.hpp>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>

class PhongHocManager {
    std::vector<ClassRoom> _dsPhong;
    std::filesystem::path  _filePath;
    bool _isDirty = false;

public:
    explicit PhongHocManager(
        std::filesystem::path filePath = "data/phonghoc.json");

    void load();
    void save();
    void saveIfDirty();

    // CRUD
    void them(const std::string& maPhong, int sucChua, RoomType loai);
    void xoa(const std::string& maPhong);

    std::optional<ClassRoom> timTheoMa(const std::string& maPhong) const;
    const std::vector<ClassRoom>& getAll() const;
    std::size_t soLuong() const;
    bool isDirty() const;
};
