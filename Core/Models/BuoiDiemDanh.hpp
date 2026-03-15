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
    const std::string& getGhiChu() const;
    std::string getGioDiemDanhStr() const;

    void setTrangThai(Status trangThai);
    void setGhiChu(const std::string& ghiChu);
};

class BuoiDiemDanh {
    DateTime _ngayDiemDanh;
    CaHoc _caDiemDanh;
    int _soTiet;
    bool _khoaDiemDanh;
    // std::vector<ChiTietDiemDanh> 

public:
    
};