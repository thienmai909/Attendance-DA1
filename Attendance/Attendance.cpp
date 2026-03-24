#include <iostream>
#include <format>

#include <DataType.hpp>
#include <Utility.hpp>
#include <AppManager.hpp>

int main() {
    AppManager attendance;
    attendance.khoiDong();

    auto dsSV = attendance.getSVManager();
    dsSV.them(SinhVien("0023412316", "Mai Hữu Thiện"));
    if (dsSV.timTheoMa("023412316").has_value())
        std::cout << std::format("tìm thấy");
}