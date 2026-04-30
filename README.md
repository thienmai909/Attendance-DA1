# 📋 Hệ Thống Điểm Danh

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.23+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![vcpkg](https://img.shields.io/badge/vcpkg-package%20manager-5C2D91?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20(WSL)-lightgrey?style=for-the-badge)

**Ứng dụng quản lý điểm danh sinh viên theo học phần, chạy trên terminal với giao diện TUI (Text User Interface).**

</div>

---

## 📌 Giới Thiệu

**Hệ Thống Điểm Danh** là một ứng dụng quản lý điểm danh sinh viên dành cho trường đại học, được xây dựng bằng **C++20** với giao diện terminal tương tác (TUI). Ứng dụng hỗ trợ hai vai trò người dùng: **Admin** và **Giảng viên**, với đầy đủ tính năng quản lý, báo cáo và xuất dữ liệu.

### ✨ Tính Năng Chính

| Nhóm | Tính năng |
|------|-----------|
| 🔐 **Xác thực** | Đăng nhập có mã hóa mật khẩu (Argon2 via libsodium) |
| 🎓 **Điểm danh** | Ghi nhận từng buổi: Có mặt / Muộn / Vắng / Chưa điểm danh |
| 🔒 **Khóa buổi** | Xác nhận + khóa buổi học đã hoàn tất |
| 📊 **Thống kê** | Tỉ lệ vắng, ngưỡng cấm thi, top SV vắng nhiều, thống kê theo ca |
| 📑 **Báo cáo** | Xuất báo cáo đa dạng ra file `.xlsx` (Excel) |
| 👥 **Quản lý** | CRUD đầy đủ: Sinh viên, Giảng viên, Lớp học phần, Phòng học |
| 🔄 **Sắp xếp** | Đa tiêu chí: % vắng, tên A→Z / Z→A, mã SV, số lớp cấm thi |
| 💾 **Lưu trữ** | Dữ liệu JSON, tự động lưu khi thoát |

---

## 🏗️ Kiến Trúc Dự Án

```
Project/
├── Core/                   # Tầng Model (dữ liệu nghiệp vụ)
│   ├── Models/
│   │   ├── SinhVien        # Model sinh viên + thông tin liên hệ
│   │   ├── GiangVien       # Model giảng viên + tài khoản
│   │   ├── LopHocPhan      # Lớp học phần + buổi điểm danh
│   │   ├── BuoiDiemDanh    # Buổi học + chi tiết từng SV
│   │   ├── DangKyHoc       # Đăng ký học (SV ↔ LHP)
│   │   └── DataType        # Kiểu dùng chung (Status, CaHoc, HocKi...)
│   └── Utils/              # Tiện ích: Logger, CSV, Utility
│
├── Manager/                # Tầng Business Logic
│   ├── AppManager          # Điều phối tổng thể + load/save JSON
│   ├── GiangVienManager    # Xác thực, CRUD giảng viên
│   ├── SinhVienManager     # CRUD sinh viên
│   ├── LopHocPhanManager   # CRUD lớp học phần
│   ├── DangKyHocManager    # Quản lý đăng ký học
│   ├── DiemDanhManager     # Logic điểm danh + tính ngưỡng
│   ├── ThongKeManager      # Thống kê + sắp xếp đa tiêu chí
│   ├── BaoCaoManager       # Xuất báo cáo XLSX
│   └── PhongHocManager     # CRUD phòng học
│
├── Ui/                     # Tầng Giao Diện (FTXUI)
│   ├── Core/               # UiHelper, UiColors
│   ├── Screen/             # Các màn hình chính
│   │   ├── LoginScreen     # Đăng nhập
│   │   ├── MainMenuScreen  # Menu chính (theo vai trò)
│   │   ├── DiemDanhScreen  # Điểm danh + xem chi tiết buổi
│   │   ├── BaoCaoScreen    # Báo cáo 5 tab (cuộn, sắp xếp)
│   │   ├── SinhVienScreen  # Quản lý sinh viên
│   │   ├── GiangVienScreen # Quản lý giảng viên
│   │   ├── LopHocPhanScreen# Quản lý lớp học phần
│   │   └── PhongHocScreen  # Quản lý phòng học
│   └── Form/               # Form thêm/sửa/xóa
│
├── Attendance/
│   ├── Attendance.cpp      # Điểm khởi chạy chính (main)
│   └── seed.cpp            # Công cụ nạp dữ liệu mẫu
│
├── data/                   # Dữ liệu JSON (tự tạo khi chạy)
├── output/                 # File XLSX xuất ra
├── logs/                   # Log ứng dụng
├── vcpkg/                  # Package manager (git submodule)
├── CMakeLists.txt
├── CMakePresets.json
└── vcpkg.json              # Danh sách dependencies
```

### 📦 Thư Viện Sử Dụng

| Thư viện | Phiên bản | Mục đích |
|----------|-----------|---------|
| [FTXUI](https://github.com/ArthurSonzogni/FTXUI) | latest | Giao diện TUI tương tác |
| [nlohmann/json](https://github.com/nlohmann/json) | latest | Đọc/ghi JSON |
| [libsodium](https://libsodium.org/) | latest | Mã hóa mật khẩu (Argon2) |
| [libxlsxwriter](https://libxlsxwriter.github.io/) | latest | Xuất file Excel (.xlsx) |
| [vincentlaucsb/csv-parser](https://github.com/vincentlaucsb/csv-parser) | 2.3.0 | Xử lý CSV |
| [minizip](https://github.com/madler/zlib) | latest | Nén/giải nén |

---

## ⚙️ Cài Đặt

### Yêu Cầu Chung

- **CMake** ≥ 3.23
- **Git** (để clone submodule vcpkg)
- **C++20** compiler:
  - Windows: Visual Studio 2022 (MSVC) hoặc clang-cl
  - Linux/WSL: GCC 13+ hoặc Clang 16+

> **Lưu ý:** Ứng dụng cần terminal hỗ trợ **Unicode** và **màu 256/True Color**.  
> Khuyến nghị dùng [Windows Terminal](https://aka.ms/terminal) với profile Ubuntu.

---

### 🪟 Cài Đặt trên Windows

#### 1. Yêu cầu
- [Visual Studio 2022](https://visualstudio.microsoft.com/) với workload **"Desktop development with C++"**
- [CMake](https://cmake.org/download/) ≥ 3.23 (hoặc dùng CMake tích hợp trong VS)
- [Git for Windows](https://git-scm.com/download/win)

#### 2. Clone project

```powershell
git clone --recurse-submodules https://github.com/thienmai909/Attendance-DA1.git
cd Attendance-DA1
```

> **Lưu ý:** `--recurse-submodules` tự động clone `vcpkg` submodule.

#### 3. Cấu hình CMake

```powershell
cmake --preset attendance-win32
```

> CMake sẽ tự động cài đặt tất cả dependencies qua vcpkg (lần đầu mất ~5–15 phút).

#### 4. Build

```powershell
# Build ứng dụng chính
cmake --build build-win32 --config Release --target Attendance

# Build công cụ nạp dữ liệu mẫu
cmake --build build-win32 --config Release --target Seed
```

#### 5. Chạy

```powershell
.\build-win32\Attendance\Release\Attendance.exe
```

---

### 🐧 Cài Đặt trên WSL Ubuntu 24/26 LTS

#### 1. Cài dependencies hệ thống

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y \
    git cmake ninja-build \
    build-essential \
    curl zip unzip tar \
    pkg-config autoconf automake libtool\
    libssl-dev \
    libgl1-mesa-dev \
    libx11-dev
```

#### 2. Clone project

```bash
git clone --recurse-submodules https://github.com/thienmai909/Attendance-DA1.git
cd Attendance-DA1
```

#### 3. Cấu hình CMake

```bash
cmake --preset attendance-unix
```

> Lần đầu chạy, vcpkg sẽ build tất cả thư viện từ source (~10–20 phút).

#### 4. Build

```bash
# Build ứng dụng chính
cmake --build build-unix --target Attendance

# Build công cụ nạp dữ liệu mẫu
cmake --build build-unix --target Seed
```

#### 5. Chạy

```bash
./build-unix/Attendance/Attendance
```

---

## 🗃️ Nạp Dữ Liệu Mẫu

Dự án đi kèm công cụ **Seed** để tạo bộ dữ liệu mẫu hoàn chỉnh:

| Loại dữ liệu | Số lượng |
|-------------|---------|
| Lớp học phần | 4 (IT2090, IT3210, IT4050, IT5030) |
| Sinh viên | 45 SV (phân bổ vào các LHP) |
| Giảng viên | 3 GV + 1 Admin |
| Phòng học | 7 phòng (các loại: Thường, Máy tính, Thí nghiệm) |
| Buổi điểm danh | ~28 buổi (2–3 tiết/buổi, đã khóa) |

### Chạy Seed

**Windows:**
```powershell
.\build-win32\Attendance\Release\Seed.exe
```

**Linux/WSL:**
```bash
./build-unix/Attendance/Seed
```

Sau khi chạy, thư mục `data/` sẽ chứa:
```
data/
├── sinhvien.json
├── giangvien.json
├── lophocphan.json
├── phong.json
└── dangkyhoc.json
```

> ⚠️ **Cảnh báo:** Seed sẽ **xóa và ghi đè** toàn bộ dữ liệu hiện có trong `data/`.

---

## 🚀 Hướng Dẫn Sử Dụng

### Đăng Nhập

Trước khi nạp dữ liệu mẫu, tài khoản admin được tạo mặc định như sau:

| Tài khoản | Mật khẩu | Vai trò |
|-----------|---------|---------|
| `admin` | `admin123` | Quản trị viên |


Sau khi nạp dữ liệu mẫu, sử dụng một trong các tài khoản sau:

| Tài khoản | Mật khẩu | Vai trò |
|-----------|---------|---------|
| `admin` | `Admin123` | Quản trị viên |
| `gvan` | `Gv12345` | Giảng viên |
| `gvbinh` | `Gv12345` | Giảng viên |
| `gvcuong` | `Gv12345` | Giảng viên |

### Phím Tắt Toàn Cục

| Phím | Chức năng |
|------|-----------|
| `↑ / ↓` | Di chuyển trong danh sách |
| `Tab` | Chuyển giữa các ô nhập |
| `Enter` | Xác nhận / Chọn |
| `Q / Esc` | Quay lại / Thoát |

### Menu Chính

```
[D] Điểm danh           → Chọn lớp → Chọn buổi → Điểm danh SV
[L] Quản lý lớp HP      → Xem/Thêm/Sửa/Xóa lớp học phần + SV
[S] Quản lý sinh viên   → (Admin) CRUD sinh viên
[G] Quản lý giảng viên  → (Admin) CRUD giảng viên  
[P] Quản lý phòng học   → (Admin) CRUD phòng học
[B] Báo cáo & thống kê  → Xem thống kê + xuất XLSX
[Q] Thoát
```

### Màn Hình Điểm Danh

1. Chọn **lớp học phần** (danh sách bên trái)
2. Chọn **buổi học** cần điểm danh
3. Dùng `↑ / ↓` chọn sinh viên, `Enter` để đổi trạng thái:
   - `CO_MAT` → `MUON` → `VANG` → `CHUA_DIEM_DANH`
4. Nhấn `L` để **khóa buổi** (sau khi khóa không thể sửa)

### Màn Hình Báo Cáo (5 Tab)

| Tab | Phím | Nội dung |
|-----|------|---------|
| Tổng quan | `1` | Thống kê tất cả lớp (Admin) |
| Chi tiết lớp | `2` | Thông tin, thống kê, top vắng, vắng theo ca |
| Danh sách SV | `3` | Toàn bộ SV + trạng thái + màu cảnh báo |
| Thống kê buổi | `4` | Chi tiết từng buổi điểm danh |
| Xuất file | `5` | Chọn loại báo cáo → xuất `.xlsx` |

**Phím tắt trong Báo cáo:**

| Phím | Chức năng |
|------|-----------|
| `S` | Cycle sắp xếp (% vắng → tên A→Z → tên Z→A → ...) |
| `J / K` | Cuộn nội dung xuống / lên |
| `X` | Xuất XLSX nhanh (trong Tab 5) |

### Xuất Báo Cáo XLSX

Vào **Tab 5 → Xuất file**, chọn loại báo cáo:

| Loại | Nội dung |
|------|---------|
| Báo cáo toàn diện | 4 sheet: SV, Buổi, Tổng quan, Cấm thi |
| Danh sách SV + thống kê | Thống kê từng sinh viên |
| Danh sách cấm thi | Chỉ SV vượt ngưỡng |
| Điểm danh từng buổi | Chi tiết từng buổi học |
| Tổng quan tất cả lớp | (Admin) So sánh các lớp |

File xuất ra lưu tại thư mục `output/`.

---

## 📐 Ngưỡng Cấm Thi

Ngưỡng mặc định là **20% tổng số tiết của khóa học**:

```
soTietVangToiDa = tongSoTiet × 0.20
```

| Mức | Điều kiện (% tổng tiết) | Hiển thị |
|-----|------------------------|---------|
| 🟢 Bình thường | ≤ 10% | OK |
| 🟡 Cần chú ý | > 10% | `[!]` vàng |
| 🔴 Nguy hiểm | > 18% | `[!!]` đỏ |
| ⛔ Cấm thi | > 20% | `[CT]` đỏ đậm |

> **Lưu ý:** Trạng thái **Muộn** được tính là **Có mặt** trong công thức cấm thi.

---

## 🗂️ Dữ Liệu & Lưu Trữ

- Dữ liệu lưu dưới dạng **JSON** trong thư mục `data/`
- Tự động **load khi khởi động** và **lưu khi thoát**
- Log hoạt động ghi vào `logs/app.log`
- Báo cáo XLSX xuất vào `output/`

---

## 🛠️ Build Targets

| Target | Mô tả |
|--------|-------|
| `Attendance` | Ứng dụng chính |
| `Seed` | Công cụ nạp dữ liệu mẫu |

---

## 📝 Ghi Chú Phát Triển

- **C++ Standard:** C++20 (requires, ranges, std::format, std::optional...)
- **Architecture:** 3-layer (Core → Manager → UI), không có dependency ngược
- **Terminal:** Cần hỗ trợ ANSI escape codes, Unicode UTF-8, ≥ 120 cột × 35 dòng

---

## 📄 License

Dự án được phát triển phục vụ mục đích học tập (Đồ án 1 — Khoa Sư phạm Toán - Tin).

---

<div align="center">
<sub>Built with ❤️ using C++20 + FTXUI</sub>
</div>
