#pragma once

#include <DataType.hpp>
#include <Utility.hpp>
#include <BuoiDiemDanh.hpp>

#include <optional>
#include <string>
#include <vector>

enum class HocKi {
    DEFAULT = 0, I, II, He
};

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

    std::vector<BuoiDiemDanh> _dsBuoiDiemDanh;

    void setSoTietDaHoc(int soTietDaHoc);
    void setSoBuoiDaHoc(int soBuoiDaHoc);

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
    std::string getHocKiStr() const;
    std::optional<ClassRoom> getPhongHoc() const;
    std::string getTenPhongHoc() const;
    const std::vector<BuoiDiemDanh>& getDsBuoiDiemDanh() const;

    void setTenLHP(const std::string& tenLHP);
    void setSoTC(int soTC);
    void setTongSoTiet(int tongSoTiet);
    void setNguongCamThi(double nguongCamThi);
    void setPhongHoc(const ClassRoom& phongHoc);
    void setPhongHoc(const std::string& tenPhong, int sucChua, RoomType loaiPhong);

    int soTietVangToiDa() const; // _tongSoTiet * _nguongCamThi;
    bool kiemTraViPham(int soTietVang) const; // kiểm tả một con số tiết vắng cụ thể có vượt ngưỡng của lớp này không
    void ghiNhanBuoiHoc(int soTiet);
    double tiLeVangHienTai(int tongSoTietVang) const;
    int soTietVangDoiDaChoPhep() const;
    std::string tienDoHocTapStr() const;

    utility_csv::Row toCSVRow() const;
    static LopHocPhan fromCSVRow(const utility_csv::Row& row);

};