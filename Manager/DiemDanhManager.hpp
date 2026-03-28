#pragma once

#include <LopHocPhanManager.hpp>

#include <LopHocPhan.hpp>
#include <SinhVien.hpp>
#include <Utility.hpp>

#include <string>
#include <vector>

class DiemDanhManager {
    LopHocPhanManager& _lhpManager;

public:
    explicit DiemDanhManager(LopHocPhanManager& lhpManager);

    void diemDanh(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::string& maSV,
        const DateTime& gio,
        Status trangThai,
        const std::string& ghiChu = ""
    );
    void diemDanhHangLoat(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::vector<std::string>& dsMaSV,
        Status tranThaiMacDinh = Status::CO_MAT
    );
    void khoaBuoiVaAutoVang(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::vector<std::string>& dsMaSVTrongLop
    );
    void capNhatTrangThai(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::string& maSV,
        Status trangThai
    );
    void khoaBuoi(const std::string& maLHP, std::size_t buoiIndex);

    // Số tiết vắng của 1 sinh viên trong 1 lớp
    int soTietVang(const std::string& maLHP, const std::string& maSV) const;
    // Tỉ lệ vắng (0.0 -> 1.0)
    double tiLeVang(const std::string& maLHP, const std::string& maSV) const;
    // Sinh viên có bị cấm thi không
    bool biCamThi(const std::string& maLHP, const std::string& maSV) const;
    // Ds SV bị cấm thi trong lớp
    std::vector<std::string> dsSVBiCamThi(const std::string& maLHP) const;
    // Còn sinh viên nào chưa điểm danh không
    std::vector<std::string> dsSVChuaDiemDanh(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::vector<std::string>& dsMaSVTrongLop
    ) const;
    // Báo cáo điểm danh 1 buổi: trả về map maSV -> Status
    std::vector<std::pair<std::string, Status>> baoCaoBuoi(
        const std::string& maLHP,
        std::size_t buoiIndex
    ) const;
    // Tổng hợp toàn lớp: maSV -> soTietVang
    std::vector<std::pair<std::string, int>> tongHopLop(
        const std::string& maLHP
    ) const;
private:
    LopHocPhan& timLop(const std::string& maLHP);
    const LopHocPhan& timLopConst(const std::string& maLHP) const;
};