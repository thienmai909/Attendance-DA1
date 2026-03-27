#pragma once

#include <LopHocPhan.hpp>
#include <Utility.hpp>

#include <vector>
#include <string>
#include <optional>
#include <filesystem>

class LopHocPhanManager {
    std::vector<LopHocPhan> _dsLHP;
    std::filesystem::path _filePath;
    bool _isDirty = false;

public:
    explicit LopHocPhanManager(std::filesystem::path filePath = "data/lophocphan.json");

    void load();
    void save();
    void saveDirty();

    void them(const LopHocPhan& lopHocPhan);
    void capNhat(const LopHocPhan& lopHocPhan);
    void xoa(const std::string& maLHP);

    std::optional<LopHocPhan> timTheoMa(const std::string& maLHP) const;
    std::vector<LopHocPhan> timTheoTen(const std::string& keyword) const;
    std::vector<LopHocPhan> locTheoHocKi(HocKi hocKi) const;

    void themBuoi(
        const std::string& maLHP,
        const DateTime& ngay,
        CaHoc ca,
        int soTiet
    );
    LopHocPhan& getLopRef(const std::string& maLHP);
    void markDirty();

    const std::vector<LopHocPhan>& getAll() const;
    std::size_t soLuong() const;
    bool isDirty() const;

private:
    std::vector<LopHocPhan>::iterator timIterator(const std::string& maLHP);
};