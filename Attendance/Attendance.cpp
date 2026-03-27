#include <iostream>
#include <format>

#include <DataType.hpp>
#include <Utility.hpp>
#include <AppManager.hpp>

int main() {
    AppManager attendance;
    attendance.khoiDong();

    auto& dsSV = attendance.getSVManager();
    auto& dsGV = attendance.getGVManager();

    try {
        GiangVien gv1 = GiangVien("0023412316", "Nguyễn Ngọc Chi");
        gv1.setTaiKhoan("ngocchi", "ngocchi123");
        gv1.setHocVi(Degree::THACSI);
        dsGV.capNhat(gv1);

    } catch (const std::invalid_argument& e) {
        std::cout << e.what();
    }
    // if (dsSV.timTheoMa("0023412244").has_value())
    //     std::cout << std::format("tìm thấy");
    // dsSV.saveIfDirty();
    attendance.dongLai();
}