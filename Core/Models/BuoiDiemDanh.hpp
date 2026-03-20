#pragma once

#include <DataType.hpp>

#include <string>
#include <optional>
#include <vector>


enum class CaHoc {
    DEFAULT = 0, SANG, CHIEU, TOI
};

enum class Status {
    DEFAULT = 0, VANG, CO_MAT, MUON
};

class ChiTietDiemDanh {
    std::string _maSV;
    std::optional<DateTime> _gioDiemDanh;
    Status _trangThai = Status::DEFAULT;
    std::string _ghiChu;

public:
    ChiTietDiemDanh(std::string maSV, DateTime gioDiemDanh, Status trangThai, std::string ghiChu = "");

    const std::string& getMaSV() const;
    Status getTrangThai() const;
    std::string getTrangThaiStr() const;
    const std::string& getGhiChu() const;
    std::string getGioDiemDanhStr() const;

    void setTrangThai(Status trangThai);
    void setGhiChu(const std::string& ghiChu);
};

// =============== Buoi Diem Danh ===============
class BuoiDiemDanh {
    std::optional<DateTime> _ngayDiemDanh;
    CaHoc _caDiemDanh;
    int _soTiet;
    bool _khoaDiemDanh = false;
    std::vector<ChiTietDiemDanh> _danhSachChiTiet;

public:
    BuoiDiemDanh(DateTime ngayDiemDanh, CaHoc caDiemDanh, int soTiet);

    std::string getNgayDiemDanhStr() const;
    std::optional<DateTime> getNgayDiemDanh() const;
    CaHoc getCaDiemDanh() const;
    std::string getCaDiemDanhStr() const;
    int getSoTiet() const;
    bool isKhoaDiemDanh() const;
    const std::vector<ChiTietDiemDanh>& getDanhSachChiTiet() const;

    void khoaBuoi();
    void themChiTiet(
        const std::string& maSV,
        const DateTime& gioDiemDanh,
        Status trangThai,
        const std::string& ghiChu
    );
    void capNhatTrangThai(const std::string& maSV, Status trangThai);
    void capNhatGhiChu(const std::string& maSV, const std::string& ghiChu);
    int demVangMat() const;
};