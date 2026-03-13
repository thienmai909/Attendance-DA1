#pragma once

#include <DataType.hpp>
#include <Utility.hpp>

#include <optional>
#include <string>

class LopHocPhan {
    std::string _maLHP;
    std::string _tenLHP;
    int _soTC;
    int _tongSoTiet;
    int _soTietDaHoc = 0;
    int _soBuoiDaHoc = 0;
    double _nguongCamThi;
    HocKi _hocKi = HocKi::DEFAULT;
    std::optional<ClassRoom> _phongHoc;
public:
    LopHocPhan() = default;
    LopHocPhan(
        std::string maLHP,
        std::string tenLHP,
        int soTC, int tongSoTiet,
        double nguongCamThi,
        HocKi hocKi
    );

    const std::string& getMaLHP() const;
    const std::string& getTenLHP() const;
    int getSoTC() const;
    int getTongSoTiet() const;
    int getSoTietDaHoc() const;
    int getSoBuoiDaHoc() const;
    double getNguongCamThi() const;
    HocKi getHocKi() const;
    std::optional<ClassRoom> getPhongHoc() const;

    void setTenLHP(const std::string& tenLHP);
    void setSoTC(int soTC);
    void setNguongCamThi(double nguongCamThi);
    void setPhongHoc(const ClassRoom& phongHoc);

    int soTietVangToiDa() const; // _tongSoTiet * _nguongCamThi;
    bool kiemTraViPham(int soTietVang) const; // kiểm tả một con số tiết vắng cụ thể có vượt ngưỡng của lớp này không
    void ghiNhanBuoiHoc(int soTiet);
    double tiLeVangHienTai(int tongSoTietVang) const;
    int soTietVangDoiDaChoPhep() const;
    std::string tienDoHocTapStr() const;

    utility_csv::Row toCSVRow() const;
    static LopHocPhan fromCSVRow(const utility_csv::Row& row);

};