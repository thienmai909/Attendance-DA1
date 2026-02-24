#include <iostream>
#include <format>
#include <models.hpp>

int main() {
    Contact conact = Contact("thienmai0637@gmail.com", "0339991772");

    std::cout << std::format("Email: {}\nPhone number: {}", conact.getEmail(), conact.getPhoneNumber()) << std::endl;

    StudyPeriod cahoc = StudyPeriod(7, 9, DateTime(2, 3, 2026, 13, 0, 0), DateTime(2, 3, 2026, 15, 30, 0));

    std::cout << std::format("{}\nTiết: {} - {}\nGiờ: {} - {}", 
            cahoc.getThuNgay(),
            cahoc.getTietBatDau(), 
            cahoc.getTietKetThuc(),
            cahoc.getTGBatDau(), 
            cahoc.getTGKetThuc())
        << std::endl;

    ClassRoom phonghoc = ClassRoom("B4-102", 45, RoomType::PhongThucHanh);
    std::cout << std::format("Phòng: {}\nSức chứa: {}\nLoại: {}",
        phonghoc.getTenPhong(), phonghoc.getSucChua(), phonghoc.getLoaiPhong());
}
