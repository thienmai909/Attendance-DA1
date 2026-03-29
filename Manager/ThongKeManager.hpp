#pragma once

#include <LopHocPhanManager.hpp>
#include <SinhVienManager.hpp>
#include <DangKyHocManager.hpp>

#include <string>
#include <vector>
#include <map>

struct ThongKeSinhVien {
    std::string maSV;
    std::string tenSV;
    int soTietVang = 0;
    int soTietMuon = 0;
    int soTietCoMat = 0;
    double tyLeVang = 0.0;
    bool biCamThi = false;
};

struct ThongKeBuoi {
    std::size_t buoiIndex;
    std::string ngay;
    std::string ca;
    int soTiet = 0;
    int soCoMat = 0;
    int soVang = 0;
    int soMuon = 0;
    double tyLeCoMat = 0.0;
};

struct ThongKeLop {
    std::string maLHP;
    std::string tenLHP;
    int soSinhVien = 0;
    int soBuoiDaHoc = 0;
    int soTietDaHoc = 0;
    double tyLeVangTrungBinh = 0.0;
    int soSVBiCamThi = 0;
};

class ThongKeManager {
    LopHocPhanManager& _lhpManager;
    SinhVienManager& _svManager;
    DangKyHocManager& _dkManager;

public:
    ThongKeManager(
        LopHocPhanManager& lhpManager,
        SinhVienManager& svManager,
        DangKyHocManager& dkManager
    );

    ThongKeSinhVien thongKeSV(
        const std::string& maLHP,
        const std::string& maSV
    ) const;
    std::vector<ThongKeSinhVien> thongKeToanLop(
        const std::string& maLHP
    ) const;
    std::vector<ThongKeSinhVien> topSVVangNhieu(
        const std::string& maLHP,
        int topN = 5
    ) const;
    std::vector<ThongKeSinhVien> dsSVBiCamThi(
        const std::string& maLHP
    ) const;

    ThongKeBuoi thongKeBuoi(
        const std::string& maLHP,
        std::size_t buoiIndex
    ) const;
    std::vector<ThongKeBuoi> thongKeTatCaBuoi(
        const std::string& maLHP
    ) const;
    std::optional<ThongKeBuoi> buoiVangCaoNhat(
        const std::string& maLHP
    ) const;

    ThongKeLop thongKeLop(const std::string& maLHP) const;
    std::vector<ThongKeLop> thongKeTatCaLop() const;
    std::optional<ThongKeLop> lopVangCaoNhat() const;

    std::map<CaHoc, double> tyLeVangTheoCa(
        const std::string& maLHP
    ) const;

    std::vector<std::pair<std::string, Status>> baoCaoBuoi(
        const std::string& maLHP,
        std::size_t buoiIndex
    ) const;
    std::vector<std::pair<std::string, int>> tongHopLop(
        const std::string& maLHP
    ) const;
private:
    const LopHocPhan& timLop(const std::string& maLHP) const;
    void tinhChiTietSV(
        const LopHocPhan& lhp,
        const std::string& maSV,
        int& soTietVang,
        int& soTietMuon,
        int& soTietCoMat
    ) const;
};