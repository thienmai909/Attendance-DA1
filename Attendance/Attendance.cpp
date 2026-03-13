#include <iostream>
#include <format>

#include <DataType.hpp>
#include <Utility.hpp>

#include <GiangVien.hpp>
#include <SinhVien.hpp>
#include <LopHocPhan.hpp>

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
        phonghoc.getTenPhong(), phonghoc.getSucChua(), phonghoc.getLoaiPhong())
        << std::endl;

    // auto a = utility_input::readInt<int>("Nhập 1 số: ");
    // std::cout << a.value() << std::endl;

    GiangVien gv("GV001", "Ngọc Chi");
    gv.setHocVi(Degree::THACSI);
    gv.setLienHe("ngocchi@gmail.com", "0394023174");
    gv.setTaiKhoan("ngocchi", "1234");
    std::cout << std::format("Tên tài khoản gv: {}\n", gv.getTenTaiKhoan());
    std::cout << std::format("Xác thực tài khoản: {}", 
        (gv.xacThucTaiKhoan("ngochi", "1234") ? "Thành công" : "Thất bại"))
        << std::endl;
    

    GiangVien gv2 = GiangVien::fromCSVRow(gv.toCSVRow());
    std::cout << "Giảng viên 2: " << std::endl;

    std::cout << std::format("Tên tài khoản gv: {}\n", gv2.getTenTaiKhoan());
    std::cout << std::format("Xác thực tài khoản: {}", 
        (gv2.xacThucTaiKhoan("ngocchi", "1234") ? "Thành công" : "Thất bại"))
        << std::endl;
    std::cout << std::format("Liên hệ: {}", gv2.getLienHeStr()) << std::endl;

    DateTime *ngaySinh = new DateTime(1, 2, 2025);
        
    SinhVien sinhvien("0023412316", "Mai Hữu Thiện");
    sinhvien.setNgaySinh(*ngaySinh);
    sinhvien.setLienHe("thienmai0637@gmail.com", "0394023174");
    std::cout << std::format("Mã SV: {}\nTên SV: {}\n", sinhvien.getMaSV(), sinhvien.getTenSV());
    std::cout << std::format("Ngày sinh: {}", sinhvien.getNgaySinhStr()) << std::endl;
    std::cout << std::format("{}", sinhvien.getLienHeStr());
    
    SinhVien sinhvien2("0023412315", "Thiện Hữu Mai");
    sinhvien2.setNgaySinh(DateTime());
    sinhvien2.setLienHe("thienyphung909@gmail.com", "0339991772");
    sinhvien2.setLopSH(LopSinhHoat::DHSTIN23B);

    SinhVien sv_output = SinhVien::fromCSVRow(sinhvien2.toCSVRow());
    std::cout << std::format("\nMã SV: {}\nTên SV: {}\n", sv_output.getMaSV(), sv_output.getTenSV());
    std::cout << std::format("Ngày sinh: {}", sv_output.getNgaySinhStr()) << std::endl;
    std::cout << std::format("{}", sv_output.getLienHeStr());
    std::cout << std::format("\nLớp sinh hoạt: {}", sv_output.getLopSH() == LopSinhHoat::DHSTIN23B ? "ĐHSTIN23B" : "NONE") ;

    LopHocPhan lophocphan = LopHocPhan("01", "Toán cao cấp 1", 3, 45, 0.2, HocKi::I);
    lophocphan.ghiNhanBuoiHoc(3);
    lophocphan.ghiNhanBuoiHoc(2);
    lophocphan.ghiNhanBuoiHoc(3);
    lophocphan.ghiNhanBuoiHoc(2);
    lophocphan.ghiNhanBuoiHoc(3);
    std::cout << "\n" << lophocphan.tienDoHocTapStr();
}