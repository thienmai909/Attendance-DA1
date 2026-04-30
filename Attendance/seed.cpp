/**
 * seed.cpp — Tao du lieu mau cho he thong quan ly diem danh
 * Build target: Seed (xem Attendance/CMakeLists.txt)
 * Chay mot lan: Seed.exe -> ghi data/*.json
 *
 * Du lieu:
 *   - 1 Admin + 3 Giang vien
 *   - 7 Phong hoc
 *   - 4 Lop hoc phan (IT2090/45t, IT2145/60t, IT3100/45t, IT3210/30t)
 *   - 45 Sinh vien (DHSTIN23A/B/C, 15 SV moi lop)
 *   - 7 buoi/LHP (6 da khoa co diem danh, 1 chua khoa)
 */

#include <AppManager.hpp>
#include <DangKyHocManager.hpp>
#include <GiangVienManager.hpp>
#include <LopHocPhanManager.hpp>
#include <PhongHocManager.hpp>
#include <SinhVienManager.hpp>

#include <BuoiDiemDanh.hpp>
#include <DataType.hpp>
#include <GiangVien.hpp>
#include <LopHocPhan.hpp>
#include <SinhVien.hpp>

#include <nlohmann/json.hpp>
#include <sodium.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------
// Tien ich ghi file
// ----------------------------------------------------------------
static void writeJson(const std::string &path, const nlohmann::json &data) {
  std::filesystem::create_directories(
      std::filesystem::path(path).parent_path());
  nlohmann::json root;
  root["data"] = data;
  std::ofstream f(path);
  f << root.dump(2);
  std::cout << "[OK] " << path << "\n";
}

// ----------------------------------------------------------------
// Tinh trang thai diem danh theo vi tri SV trong lop
//   svIdx  : thu tu SV trong danh sach dang ky (0-indexed)
//   buoiIdx: thu tu buoi (0-5 = 6 buoi da khoa)
//   total  : tong so SV dang ky lop nay
// ----------------------------------------------------------------
static Status getStatus(int svIdx, int buoiIdx, int total) {
  float pct = static_cast<float>(svIdx) / total;

  if (pct < 0.60f) {
    // ~60% binh thuong
    if (svIdx % 5 == 2 && buoiIdx == 0)
      return Status::MUON;
    if (svIdx % 7 == 4 && buoiIdx == 5)
      return Status::VANG;
    return Status::CO_MAT;
  } else if (pct < 0.78f) {
    // ~18% can chu y: vang 2 buoi (B1, B3)
    if (buoiIdx == 1 || buoiIdx == 3)
      return Status::VANG;
    if (svIdx % 3 == 0 && buoiIdx == 0)
      return Status::MUON;
    return Status::CO_MAT;
  } else if (pct < 0.90f) {
    // ~12% nguy hiem: vang 3 buoi le (B1,B3,B5)
    if (buoiIdx % 2 == 1)
      return Status::VANG;
    return Status::CO_MAT;
  } else {
    // ~10% bi cam thi: vang 5/6 buoi, chi co mat buoi B2
    return (buoiIdx == 2) ? Status::CO_MAT : Status::VANG;
  }
}

// Gio diem danh theo ca
static DateTime sessionTime(const DateTime &ngay, CaHoc ca) {
  int h = (ca == CaHoc::SANG) ? 7 : (ca == CaHoc::CHIEU) ? 13 : 17;
  return DateTime(ngay.day(), ngay.month(), ngay.year(), h, 30, 0);
}

// ----------------------------------------------------------------
// main
// ----------------------------------------------------------------
int main() {
  if (sodium_init() < 0) {
    std::cerr << "[ERR] sodium_init() failed\n";
    return 1;
  }
  std::filesystem::create_directories("data");

  // ============================================================
  // 1. GIANG VIEN
  // ============================================================
  struct GVInfo {
    std::string ma, ten, user, pass;
    int hocVi;
    bool admin;
    std::string email, sdt;
  };
  std::vector<GVInfo> gvInfos = {
      {"GV001", "Nguyễn Văn An", "gvan", "Gv12345", 2, false,
       "an.nv@school.edu.vn", "0901234501"},
      {"GV002", "Trần Thị Bình", "gvbinh", "Gv12345", 3, false,
       "binh.tt@school.edu.vn", "0901234502"},
      {"GV003", "Lê Hoàng Cường", "gvcuong", "Gv12345", 2, false,
       "cuong.lh@school.edu.vn", "0901234503"},
      {"ADMIN", "Quản Trị Viên", "admin", "Admin123", 0, true,
       "admin@school.edu.vn", "0900000000"},
  };

  nlohmann::json jGV = nlohmann::json::array();
  for (auto &g : gvInfos) {
    GiangVien gv(g.ma, g.ten, g.admin);
    gv.setTaiKhoan(g.user, g.pass); // hash qua libsodium
    gv.setHocVi(static_cast<Degree>(g.hocVi));
    gv.setLienHe(g.email, g.sdt);
    jGV.push_back(gv.toJson());
  }
  writeJson("data/giangvien.json", jGV);

  // ============================================================
  // 2. PHONG HOC (7 phong)
  // ============================================================
  // loai: 0=LyThuyet, 1=ThucHanh  (theo PhongHocManager::phongToJson)
  nlohmann::json jPH = nlohmann::json::array();
  std::vector<std::tuple<std::string, int, int>> phongs = {
      {"P101", 50, 0},   {"P102", 50, 0},   {"P103", 60, 0}, {"P201", 45, 0},
      {"Lab301", 30, 1}, {"Lab302", 30, 1}, {"H401", 80, 0},
  };
  for (auto &[ma, sc, loai] : phongs)
    jPH.push_back({{"maPhong", ma}, {"sucChua", sc}, {"loai", loai}});
  writeJson("data/phonghoc.json", jPH);

  // ============================================================
  // 3. SINH VIEN (45 SV)
  // ============================================================
  struct SVInfo {
    std::string ma, ten;
    int lsh;
    int nd, nm, ny;
  };
  std::vector<SVInfo> svInfos = {
      // DHSTIN23A (lopSinhHoat=1): SV01-SV15
      {"0023410001", "Nguyễn Minh Tuấn", 1, 5, 3, 2005},
      {"0023410002", "Trần Thị Lan", 1, 12, 7, 2005},
      {"0023410003", "Lê Văn Hưng", 1, 20, 1, 2005},
      {"0023410004", "Phạm Thị Hoa", 1, 8, 9, 2005},
      {"0023410005", "Hoàng Văn Đức", 1, 15, 4, 2005},
      {"0023410006", "Ngô Thị Mai", 1, 22, 6, 2005},
      {"0023410007", "Vũ Văn Thắng", 1, 3, 8, 2005},
      {"0023410008", "Đặng Thị Thu", 1, 18, 2, 2005},
      {"0023410009", "Bùi Văn Nam", 1, 10, 5, 2005},
      {"0023410010", "Đinh Thị Phương", 1, 25, 10, 2005},
      {"0023410011", "Trịnh Văn Long", 1, 7, 11, 2005},
      {"0023410012", "Lý Thị Hương", 1, 14, 3, 2005},
      {"0023410013", "Dương Văn Khoa", 1, 1, 12, 2005},
      {"0023410014", "Phan Thị Ngọc", 1, 19, 6, 2005},
      {"0023410015", "Cao Văn Minh", 1, 28, 8, 2005},
      // DHSTIN23B (lopSinhHoat=2): SV16-SV30
      {"0023410016", "Lưu Thị Thanh", 2, 6, 1, 2005},
      {"0023410017", "Tô Văn Bình", 2, 13, 4, 2005},
      {"0023410018", "Hồ Thị Linh", 2, 21, 7, 2005},
      {"0023410019", "Trương Văn Tùng", 2, 9, 10, 2005},
      {"0023410020", "Vương Thị Cẩm", 2, 16, 2, 2005},
      {"0023410021", "Lê Văn Đạt", 2, 24, 5, 2005},
      {"0023410022", "Nguyễn Thị Yến", 2, 2, 9, 2005},
      {"0023410023", "Phạm Văn Sơn", 2, 11, 11, 2005},
      {"0023410024", "Mai Thị Dung", 2, 29, 3, 2005},
      {"0023410025", "Đỗ Văn Hiếu", 2, 17, 6, 2005},
      {"0023410026", "Châu Thị Anh", 2, 4, 8, 2005},
      {"0023410027", "Võ Văn Quân", 2, 23, 1, 2005},
      {"0023410028", "Tạ Thị Xuân", 2, 30, 4, 2005},
      {"0023410029", "Lương Văn Hải", 2, 8, 7, 2005},
      {"0023410030", "Kiều Thị Loan", 2, 15, 10, 2005},
      // DHSTIN23C (lopSinhHoat=3): SV31-SV45
      {"0023410031", "Đoàn Văn Phúc", 3, 22, 2, 2005},
      {"0023410032", "Nông Thị Thảo", 3, 7, 5, 2005},
      {"0023410033", "Mạc Văn Cương", 3, 14, 8, 2005},
      {"0023410034", "La Thị Hằng", 3, 1, 11, 2005},
      {"0023410035", "Ông Văn Tài", 3, 19, 3, 2005},
      {"0023410036", "Huỳnh Thị Hậu", 3, 26, 6, 2005},
      {"0023410037", "Lâm Văn Nghĩa", 3, 10, 9, 2005},
      {"0023410038", "Thái Thị Nhung", 3, 18, 12, 2005},
      {"0023410039", "Trần Văn Bảo", 3, 3, 4, 2005},
      {"0023410040", "Nguyễn Thị Diệu", 3, 12, 7, 2005},
      {"0023410041", "Lê Văn Phong", 3, 20, 1, 2005},
      {"0023410042", "Phạm Thị Quỳnh", 3, 28, 5, 2005},
      {"0023410043", "Hoàng Văn Sang", 3, 5, 8, 2005},
      {"0023410044", "Vũ Thị Tú", 3, 11, 10, 2005},
      {"0023410045", "Đặng Văn Uy", 3, 25, 2, 2005},
  };

  nlohmann::json jSV = nlohmann::json::array();
  int svIdx = 0;
  for (auto &s : svInfos) {
    SinhVien sv(s.ma, s.ten);
    sv.setLopSH(static_cast<LopSinhHoat>(s.lsh));
    sv.setNgaySinh(s.nd, s.nm, s.ny);

    // Tao lien he: email theo ma SV, SĐT ngau nhien but deterministic
    char email[64], sdt[16];
    std::snprintf(email, sizeof(email), "%s@std.edu.vn", s.ma.c_str());
    std::snprintf(sdt,   sizeof(sdt),   "09%08d", svIdx * 1234567 % 100000000);
    sv.setLienHe(std::string(email), std::string(sdt));

    jSV.push_back(sv.toJson());
    ++svIdx;
  }
  writeJson("data/sinhvien.json", jSV);

  // ============================================================
  // 4. LOP HOC PHAN + BUOI DIEM DANH
  // ============================================================
  // Danh sach SV dang ky moi LHP (ma SV)
  // IT2090: SV01-SV25 (25 SV)
  // IT2145: SV11-SV30 (20 SV)
  // IT3100: SV16-SV33 (18 SV)
  // IT3210: SV01-SV10 + SV34-SV45 (10+12=22 SV)

  auto makeRange = [&](int from, int to) {
    std::vector<std::string> v;
    for (int i = from; i <= to; ++i) {
      char buf[16];
      std::snprintf(buf, sizeof(buf), "002341%04d", i);
      v.push_back(buf);
    }
    return v;
  };

  struct LHPDef {
    std::string ma, ten, maGV, tenPhong;
    int soTC, tongSoTiet, hocKi; // hocKi: 1=I, 2=II
    double nguong;
    int soTietPerBuoi;
    CaHoc ca;
    // ngay bat dau buoi 1 (day, month, year)
    int d0, m0, y0;
    std::vector<std::string> svList;
  };

  // Buoi: cach nhau 7 ngay, moi LHP bat dau tu ngay khac nhau
  auto makeDates = [](int d0, int m0, int y0) {
    std::vector<DateTime> dates;
    DateTime cur(d0, m0, y0);
    for (int i = 0; i < 7; ++i) {
      dates.push_back(cur);
      cur = cur.addDays(7);
    }
    return dates;
  };

  // IT2090 & IT3100: 45t, 3t/buoi; IT2145: 60t, 3t/buoi; IT3210: 30t, 2t/buoi
  auto svIT2090 = makeRange(1, 25);
  auto svIT2145 = makeRange(11, 30);
  auto svIT3100 = makeRange(16, 33);
  std::vector<std::string> svIT3210 = makeRange(1, 10);
  auto tail = makeRange(34, 45);
  svIT3210.insert(svIT3210.end(), tail.begin(), tail.end());

  std::vector<LHPDef> lhpDefs = {
      {"IT2090", "Lập trình C++", "GV001", "P101", 3, 45, 1, 0.20, 3,
       CaHoc::SANG, 9, 2, 2026, svIT2090},
      {"IT2145", "Cấu trúc DL & GT", "GV002", "P102", 3, 60, 1, 0.20, 3,
       CaHoc::CHIEU, 10, 2, 2026, svIT2145},
      {"IT3100", "Cơ sở dữ liệu", "GV003", "Lab301", 3, 45, 2, 0.20, 3,
       CaHoc::SANG, 11, 2, 2026, svIT3100},
      {"IT3210", "Mạng máy tính", "GV001", "P201", 2, 30, 2, 0.20, 2,
       CaHoc::TOI, 12, 2, 2026, svIT3210},
  };

  nlohmann::json jLHP = nlohmann::json::array();
  nlohmann::json jDK = nlohmann::json::array();

  for (auto &def : lhpDefs) {
    LopHocPhan lhp(def.ma, def.ten, def.soTC, def.tongSoTiet, def.nguong,
                   static_cast<HocKi>(def.hocKi));
    lhp.setMaGV(def.maGV);
    // Phan phong hoc: phai khop trong phonghoc.json
    // Lay loai tu phongs
    int loai = 0;
    for (auto &[mp, sc, lo] : phongs)
      if (mp == def.tenPhong) {
        loai = lo;
        break;
      }
    lhp.setPhongHoc(def.tenPhong, 50,
                    loai == 1 ? RoomType::PhongThucHanh
                              : RoomType::PhongLyThuyet);

    auto dates = makeDates(def.d0, def.m0, def.y0);
    int total = (int)def.svList.size();

    for (int b = 0; b < 7; ++b) {
      lhp.themBuoiDiemDanh(dates[b], def.ca, def.soTietPerBuoi);
      auto &buoi = lhp.getBuoi(b);

      if (b < 6) {
        // 6 buoi da khoa: diem danh tung SV
        DateTime gio = sessionTime(dates[b], def.ca);
        for (int s = 0; s < total; ++s) {
          Status st = getStatus(s, b, total);
          buoi.themChiTiet(def.svList[s], gio, st, "", false);
        }
        buoi.khoaBuoi();
        lhp.ghiNhanBuoiHoc(def.soTietPerBuoi);
      }
      // Buoi 7: chua khoa, chua diem danh (de nguyen)
    }

    jLHP.push_back(lhp.toJson());

    // Dang ky hoc cho LHP nay
    for (auto &maSV : def.svList) {
      jDK.push_back({{"maSV", maSV},
                     {"maLopHocPhan", def.ma},
                     {"ngayDangKy", "01/02/2026"},
                     {"isActive", true}});
    }
  }

  writeJson("data/lophocphan.json", jLHP);
  writeJson("data/dangkyhoc.json", jDK);

  // ============================================================
  // Tong ket
  // ============================================================
  std::cout << "\n=== SEED HOÀN TẤT ===\n";
  std::cout << "Giảng viên : 4 (3 GV + 1 Admin)\n";
  std::cout << "Phòng học  : 7\n";
  std::cout << "Sinh viên  : 45\n";
  std::cout << "Lop HP     : 4\n";
  std::cout << "Đăng ký    : " << jDK.size() << " bản ghi\n";
  std::cout << "\n Tài khoản:\n";
  std::cout << "  admin  / Admin123   [Admin]\n";
  std::cout << "  gvan   / Gv12345    [GV001 - day IT2090, IT3210]\n";
  std::cout << "  gvbinh / Gv12345    [GV002 - day IT2145]\n";
  std::cout << "  gvcuong/ Gv12345    [GV003 - day IT3100]\n";

  return 0;
}
