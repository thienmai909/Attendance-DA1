#include <iostream>
#include <format>

#include <DataType.hpp>
#include <Utility.hpp>
#include <AppManager.hpp>

int main() {
    AppManager attendance;
    attendance.khoiDong();

    auto& svManager = attendance.getSVManager();
    auto& gvManager = attendance.getGVManager();
    auto& lhpManager = attendance.getLHPManager();

    try {
        // gvManager.them(GiangVien("002", "Nguyễn Trọng Nhân"));
        // LopHocPhan lhp1("IN4312", "Lập trình hướng đối tượng", 3, 45, 0.2, HocKi::I);
        // GiangVien& gv1 = gvManager.getGiangVienRef("001");
        // // gv1.setTaiKhoan("ngocchi", "ngocchi123");

        // lhp1.setMaGV(gv1.getMaGV());
        // lhpManager.them(lhp1);
        // lhpManager.themBuoi("IN4312", DateTime(), CaHoc::SANG, 3);
        // lhpManager.themBuoi("IN4312", DateTime(), CaHoc::CHIEU, 2);
        GiangVien& gv = gvManager.getGiangVienRef("002");
        gv.setTaiKhoan("trongnhan", "trongnhan123");
        gvManager.capNhat(gv);

    } catch (const std::invalid_argument& e) {
        std::cout << e.what();
    }
    // if (dsSV.timTheoMa("0023412244").has_value())
    //     std::cout << std::format("tìm thấy");
    // dsSV.saveIfDirty();
    attendance.dongLai();
}