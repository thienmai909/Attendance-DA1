#include <BaoCaoManager.hpp>
#include <xlsxwriter.h>
#include <chrono>
#include <ctime>
#include <iostream>

BaoCaoManager::BaoCaoManager(
    ThongKeManager &tkManager,
    std::filesystem::path outputDir
) : _tkManager(tkManager)
  , _outputDir(std::move(outputDir))
{}

void BaoCaoManager::xuatBaoCaoBuoi(
    const std::string &maLHP,
    std::size_t buoiIndex,
    DinhDangXuat dinhDang
) {
    ensureOutputDir();
    auto path = taoTenFile(maLHP, "buoi_" + std::to_string(buoiIndex + 1), dinhDang);
    if (dinhDang == DinhDangXuat::CSV)
        xuatBuoiCSV(maLHP, buoiIndex, path);
    else if (dinhDang == DinhDangXuat::JSON)
        xuatBuoiJSON(maLHP, buoiIndex, path);
    else
        xuatTatCaBuoiXLSX(maLHP, path); // fallback sang tat ca buoi
    std::cout << "Đã xuất: " << path.string() << "\n";
}

void BaoCaoManager::xuatBaoCaoTatCaBuoi(
    const std::string &maLHP,
    DinhDangXuat dinhDang
) {
    ensureOutputDir();
    auto path = taoTenFile(maLHP, "tatca_buoi", dinhDang);
    if (dinhDang == DinhDangXuat::CSV)
        xuatTatCaBuoiCSV(maLHP, path);
    else if (dinhDang == DinhDangXuat::JSON)
        xuatTatCaBuoiJSON(maLHP, path);
    else
        xuatTatCaBuoiXLSX(maLHP, path);
    std::cout << "Đã xuất: " << path.string() << "\n";
}

void BaoCaoManager::xuatBaoCaoSinhVien(
    const std::string &maLHP,
    DinhDangXuat dinhDang
) {
    ensureOutputDir();
    auto path = taoTenFile(maLHP, "sinhvien", dinhDang);
    if (dinhDang == DinhDangXuat::CSV)
        xuatSinhVienCSV(maLHP, path);
    else if (dinhDang == DinhDangXuat::JSON)
        xuatSinhVienJSON(maLHP, path);
    else
        xuatSinhVienXLSX(maLHP, path);
    std::cout << "Đã xuất: " << path.string() << "\n";
}

void BaoCaoManager::xuatDanhSachCamThi(
    const std::string &maLHP,
    DinhDangXuat dinhDang
) {
    ensureOutputDir();
    auto path = taoTenFile(maLHP, "camthi", dinhDang);
    if (dinhDang == DinhDangXuat::CSV)
        xuatCamThiCSV(maLHP, path);
    else if (dinhDang == DinhDangXuat::JSON)
        xuatCamThiJSON(maLHP, path);
    else
        xuatCamThiXLSX(maLHP, path);
    std::cout << "Đã xuất: " << path.string() << "\n";
}

void BaoCaoManager::xuatBaoCaoTongQuan(DinhDangXuat dinhDang) {
    ensureOutputDir();
    auto path = taoTenFile("tongquan", dinhDang);
    if (dinhDang == DinhDangXuat::CSV)
        xuatTongQuanCSV(path);
    else if (dinhDang == DinhDangXuat::JSON)
        xuatTongQuanJSON(path);
    else
        xuatTongQuanXLSX(path);
    std::cout << "Đã xuất: " << path.string() << "\n";
}

std::string BaoCaoManager::xuatBaoCaoToanDien(const std::string &maLHP) {
    ensureOutputDir();
    auto path = taoTenFileXLSX(maLHP, "baocao_toanDien");
    xuatToanDienXLSX(maLHP, path);
    return path.string();
}

void BaoCaoManager::ensureOutputDir() {
    if (!std::filesystem::exists(_outputDir))
        std::filesystem::create_directories(_outputDir);
}

std::string BaoCaoManager::taoTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tm);
    return buf;
}

std::filesystem::path BaoCaoManager::taoTenFile(
    const std::string &maLHP, const std::string &loai, DinhDangXuat dinhDang
) {
    std::string ext = (dinhDang == DinhDangXuat::CSV)  ? ".csv"
                    : (dinhDang == DinhDangXuat::JSON) ? ".json"
                    :                                    ".xlsx";
    return _outputDir / (maLHP + "_" + loai + "_" + taoTimestamp() + ext);
}

std::filesystem::path BaoCaoManager::taoTenFile(
    const std::string &loai, DinhDangXuat dinhDang
) {
    std::string ext = (dinhDang == DinhDangXuat::CSV)  ? ".csv"
                    : (dinhDang == DinhDangXuat::JSON) ? ".json"
                    :                                    ".xlsx";
    return _outputDir / (loai + "_" + taoTimestamp() + ext);
}

std::filesystem::path BaoCaoManager::taoTenFileXLSX(
    const std::string &maLHP, const std::string &loai
) {
    return _outputDir / (maLHP + "_" + loai + "_" + taoTimestamp() + ".xlsx");
}

void BaoCaoManager::xuatBuoiCSV(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::filesystem::path &filePath
) {
    auto tkB = _tkManager.thongKeBuoi(maLHP, buoiIndex);
    auto bcB = _tkManager.baoCaoBuoi(maLHP, buoiIndex);

    utility_csv::CSVData data;
    data.headers = {"MaSV", "TrangThai"};
    for (const auto& [maSV, status] : bcB)
        data.rows.push_back({ maSV, statusToStr(status) });
    utility_csv::write_csv(filePath, data);
}

void BaoCaoManager::xuatTatCaBuoiCSV(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsBuoi = _tkManager.thongKeTatCaBuoi(maLHP);

    utility_csv::CSVData data;
    data.headers = {
        "Buoi", "Ngay", "Ca", "SoTiet",
        "CoMat", "Vang", "Muon", "TyLeCoMat"
    };
    for (const auto& buoi : dsBuoi)
        data.rows.push_back({
            std::to_string(buoi.buoiIndex + 1),
            buoi.ngay,
            buoi.ca,
            std::to_string(buoi.soTiet),
            std::to_string(buoi.soCoMat),
            std::to_string(buoi.soVang),
            std::to_string(buoi.soMuon),
            std::to_string(static_cast<int>(buoi.tyLeCoMat * 100)) + "%"
        });
    utility_csv::write_csv(filePath, data);
}

void BaoCaoManager::xuatSinhVienCSV(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.thongKeToanLop(maLHP);

    utility_csv::CSVData data;
    data.headers = {
        "MaSV", "HoTen",
        "SoTietCoMat", "SoTietVang", "SoTietMuon",
        "TyLeVang", "BiCamThi"
    };
    for (const auto& sinhVien : dsSV) 
        data.rows.push_back({
            sinhVien.maSV,
            sinhVien.tenSV,
            std::to_string(sinhVien.soTietCoMat),
            std::to_string(sinhVien.soTietVang),
            std::to_string(sinhVien.soTietMuon),
            std::to_string(static_cast<int>(sinhVien.tyLeVang * 100)) + "%",
            sinhVien.biCamThi ? "Co" : "Khong"
        });
    utility_csv::write_csv(filePath, data);
}

void BaoCaoManager::xuatCamThiCSV(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.dsSVBiCamThi(maLHP);

    utility_csv::CSVData data;
    data.headers = {
        "MaSV", "HoTen", "SoTietVang", "TyLeVang"
    };
    for (const auto& sinhVien : dsSV) 
        data.rows.push_back({
            sinhVien.maSV,
            sinhVien.tenSV,
            std::to_string(sinhVien.soTietVang),
            std::to_string(static_cast<int>(sinhVien.tyLeVang * 100)) + "%"
        });
    utility_csv::write_csv(filePath, data);
}

void BaoCaoManager::xuatTongQuanCSV(
    const std::filesystem::path &filePath
) {
    auto dsLop = _tkManager.thongKeTatCaLop();

    utility_csv::CSVData data;
    data.headers = {
        "MaLHP", "TenLHP", "SoSV",
        "SoBuoi", "SoTiet",
        "TyLeVangTB", "SoSVCamThi"
    };
    for (const auto& lop : dsLop)
        data.rows.push_back({
            lop.maLHP,
            lop.tenLHP,
            std::to_string(lop.soSinhVien),
            std::to_string(lop.soBuoiDaHoc),
            std::to_string(lop.soTietDaHoc),
            std::to_string(static_cast<int>(lop.tyLeVangTrungBinh * 100)) + "%",
            std::to_string(lop.soSVBiCamThi)
        });
    utility_csv::write_csv(filePath, data);
}

void BaoCaoManager::xuatBuoiJSON(
    const std::string &maLHP,
    std::size_t buoiIndex,
    const std::filesystem::path &filePath
) {
    auto tkB = _tkManager.thongKeBuoi(maLHP, buoiIndex);
    auto bcB = _tkManager.baoCaoBuoi(maLHP, buoiIndex);

    nlohmann::json j;
    j["maLHP"]     = maLHP;
    j["buoi"]      = buoiIndex + 1;
    j["ngay"]      = tkB.ngay;
    j["ca"]        = tkB.ca;
    j["soTiet"]    = tkB.soTiet;
    j["soCoMat"]   = tkB.soCoMat;
    j["soVang"]    = tkB.soVang;
    j["soMuon"]    = tkB.soMuon;
    j["tyLeCoMat"] = tkB.tyLeCoMat;
    j["chiTiet"]   = nlohmann::json::array();

    for (const auto& [maSV, status] : bcB) 
        j["chiTiet"].push_back({
            {"maSV", maSV},
            {"trangThai", statusToStr(status)}
        });
    utility_json::write_json(filePath, j);
}

void BaoCaoManager::xuatTatCaBuoiJSON(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsBuoi = _tkManager.thongKeTatCaBuoi(maLHP);

    nlohmann::json j;
    j["maLHP"] = maLHP;
    j["buoi"] = nlohmann::json::array();

    for (const auto& buoi : dsBuoi) 
        j["buoi"].push_back({
            {"buoi", buoi.buoiIndex + 1},
            {"ngay", buoi.ngay},
            {"ca", buoi.ca},
            {"soTiet", buoi.soTiet},
            {"soCoMat", buoi.soCoMat},
            {"soVang", buoi.soVang},
            {"soMuon", buoi.soMuon},
            {"tyLeCoMat", buoi.tyLeCoMat}
        });

    utility_json::write_json(filePath, j);
}

void BaoCaoManager::xuatSinhVienJSON(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.thongKeToanLop(maLHP);

    nlohmann::json j;
    j["maLHP"] = maLHP;
    j["sinhVien"] = nlohmann::json::array();

    for (const auto& sinhVien : dsSV)
        j["sinhVien"].push_back({
            {"maSV", sinhVien.maSV},
            {"tenSV", sinhVien.tenSV},
            {"soTietCoMat", sinhVien.soTietCoMat},
            {"soTietVang", sinhVien.soTietVang},
            {"soTietMuon", sinhVien.soTietMuon},
            {"tyLeVang", sinhVien.tyLeVang},
            {"biCamThi", sinhVien.biCamThi}
        });
    utility_json::write_json(filePath, j);
}

void BaoCaoManager::xuatCamThiJSON(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.dsSVBiCamThi(maLHP);

    nlohmann::json j;
    j["maLHP"]   = maLHP;
    j["camThi"]  = nlohmann::json::array();

    for (const auto& sinhVien : dsSV)
        j["camThi"].push_back({
            {"maSV",       sinhVien.maSV},
            {"tenSV",      sinhVien.tenSV},
            {"soTietVang", sinhVien.soTietVang},
            {"tyLeVang",   sinhVien.tyLeVang}
        });
    
    utility_json::write_json(filePath, j);
}

void BaoCaoManager::xuatTongQuanJSON(
    const std::filesystem::path &filePath
) {
    auto dsLop = _tkManager.thongKeTatCaLop();

    nlohmann::json j;
    j["tongQuan"] = nlohmann::json::array();

    for (const auto& lop : dsLop)
        j["tongQuan"].push_back({
            {"maLHP",            lop.maLHP},
            {"tenLHP",           lop.tenLHP},
            {"soSinhVien",       lop.soSinhVien},
            {"soBuoiDaHoc",      lop.soBuoiDaHoc},
            {"soTietDaHoc",      lop.soTietDaHoc},
            {"tyLeVangTrungBinh",lop.tyLeVangTrungBinh},
            {"soSVBiCamThi",     lop.soSVBiCamThi}
        });
    
    utility_json::write_json(filePath, j);
}

// ============================================================
// XLSX WRITERS
// ============================================================

// Macro tien ich: viet header row
static void writeHeaderRow(lxw_worksheet* ws, lxw_format* fmt,
                           const std::vector<std::string>& headers,
                           double colWidth = 20.0) {
    for (int c = 0; c < (int)headers.size(); ++c) {
        worksheet_write_string(ws, 0, (lxw_col_t)c,
                               headers[c].c_str(), fmt);
        worksheet_set_column(ws, (lxw_col_t)c, (lxw_col_t)c, colWidth, nullptr);
    }
}

void BaoCaoManager::xuatSinhVienXLSX(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.thongKeToanLop(maLHP);
    lxw_workbook  *wb = workbook_new(filePath.string().c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Danh Sách SV");

    // Formats
    lxw_format *fHeader = workbook_add_format(wb);
    format_set_bold(fHeader);
    format_set_bg_color(fHeader, 0x2E75B6); // xanh duong
    format_set_font_color(fHeader, LXW_COLOR_WHITE);
    format_set_border(fHeader, LXW_BORDER_THIN);
    format_set_align(fHeader, LXW_ALIGN_CENTER);

    lxw_format *fOk   = workbook_add_format(wb);
    format_set_border(fOk, LXW_BORDER_THIN);

    lxw_format *fWarn = workbook_add_format(wb); // > 50% nguong
    format_set_bg_color(fWarn, 0xFFFF99); // vang
    format_set_border(fWarn, LXW_BORDER_THIN);

    lxw_format *fDanger = workbook_add_format(wb); // > 80%
    format_set_bg_color(fDanger, 0xFFCC99); // cam
    format_set_border(fDanger, LXW_BORDER_THIN);

    lxw_format *fCamThi = workbook_add_format(wb);
    format_set_bg_color(fCamThi, 0xFF0000); // do
    format_set_font_color(fCamThi, LXW_COLOR_WHITE);
    format_set_bold(fCamThi);
    format_set_border(fCamThi, LXW_BORDER_THIN);

    lxw_format *fPct = workbook_add_format(wb);
    format_set_border(fPct, LXW_BORDER_THIN);
    format_set_num_format(fPct, "0.0%");

    writeHeaderRow(ws, fHeader, {
        "STT", "Mã SV", "Họ Tên",
        "Tiết Vắng", "Tiết Muộn", "Tiết Có Mặt",
        "% Vắng", "Trạng Thái"
    });
    worksheet_set_column(ws, 0, 0, 6,  nullptr); // STT
    worksheet_set_column(ws, 1, 1, 12, nullptr); // Ma SV
    worksheet_set_column(ws, 2, 2, 30, nullptr); // Ho Ten
    worksheet_set_column(ws, 6, 6, 10, nullptr); // % Vang

    for (int i = 0; i < (int)dsSV.size(); ++i) {
        const auto &sv = dsSV[i];
        lxw_row_t row = (lxw_row_t)(i + 1);

        lxw_format *fRow = sv.biCamThi   ? fCamThi
                         : sv.tyLeVang > 0.8 ? fDanger
                         : sv.tyLeVang > 0.5 ? fWarn
                         :                      fOk;

        worksheet_write_number(ws, row, 0, i + 1, fRow);
        worksheet_write_string(ws, row, 1, sv.maSV.c_str(),   fRow);
        worksheet_write_string(ws, row, 2, sv.tenSV.c_str(),  fRow);
        worksheet_write_number(ws, row, 3, sv.soTietVang,     fRow);
        worksheet_write_number(ws, row, 4, sv.soTietMuon,     fRow);
        worksheet_write_number(ws, row, 5, sv.soTietCoMat,    fRow);
        worksheet_write_number(ws, row, 6, sv.tyLeVang,       fPct);

        std::string tt = sv.biCamThi       ? "CẤM THI"
                       : sv.tyLeVang > 0.8 ? "Nguy hiểm"
                       : sv.tyLeVang > 0.5 ? "Cần chú ý"
                       :                      "Bình thường";
        worksheet_write_string(ws, row, 7, tt.c_str(), fRow);
    }
    workbook_close(wb);
}

void BaoCaoManager::xuatTatCaBuoiXLSX(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsBuoi = _tkManager.thongKeTatCaBuoi(maLHP);
    auto buoiMax = _tkManager.buoiVangCaoNhat(maLHP);
    std::size_t maxIdx = buoiMax.has_value() ? buoiMax->buoiIndex : SIZE_MAX;

    lxw_workbook  *wb = workbook_new(filePath.string().c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Điểm Danh Buổi");

    lxw_format *fHeader = workbook_add_format(wb);
    format_set_bold(fHeader); format_set_bg_color(fHeader, 0x2E75B6);
    format_set_font_color(fHeader, LXW_COLOR_WHITE);
    format_set_border(fHeader, LXW_BORDER_THIN);
    format_set_align(fHeader, LXW_ALIGN_CENTER);

    lxw_format *fNormal = workbook_add_format(wb);
    format_set_border(fNormal, LXW_BORDER_THIN);

    lxw_format *fHighlight = workbook_add_format(wb); // buoi vang cao nhat
    format_set_bg_color(fHighlight, 0xFF9999);
    format_set_border(fHighlight, LXW_BORDER_THIN);

    lxw_format *fPct = workbook_add_format(wb);
    format_set_border(fPct, LXW_BORDER_THIN);
    format_set_num_format(fPct, "0.0%");
    lxw_format *fPctHL = workbook_add_format(wb);
    format_set_bg_color(fPctHL, 0xFF9999);
    format_set_border(fPctHL, LXW_BORDER_THIN);
    format_set_num_format(fPctHL, "0.0%");

    writeHeaderRow(ws, fHeader, {
        "Buổi", "Ngày", "Ca", "Số Tiết",
        "Có Mặt", "Vắng", "Muộn", "% Có Mặt"
    });
    worksheet_set_column(ws, 1, 1, 14, nullptr);
    worksheet_set_column(ws, 7, 7, 10, nullptr);

    for (int i = 0; i < (int)dsBuoi.size(); ++i) {
        const auto &b = dsBuoi[i];
        lxw_row_t row = (lxw_row_t)(i + 1);
        bool isMax = (b.buoiIndex == maxIdx);
        lxw_format *fR  = isMax ? fHighlight : fNormal;
        lxw_format *fP  = isMax ? fPctHL     : fPct;

        worksheet_write_number(ws, row, 0, (int)b.buoiIndex + 1, fR);
        worksheet_write_string(ws, row, 1, b.ngay.c_str(), fR);
        worksheet_write_string(ws, row, 2, b.ca.c_str(),   fR);
        worksheet_write_number(ws, row, 3, b.soTiet,       fR);
        worksheet_write_number(ws, row, 4, b.soCoMat,      fR);
        worksheet_write_number(ws, row, 5, b.soVang,       fR);
        worksheet_write_number(ws, row, 6, b.soMuon,       fR);
        worksheet_write_number(ws, row, 7, b.tyLeCoMat,    fP);
    }
    workbook_close(wb);
}

void BaoCaoManager::xuatCamThiXLSX(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV = _tkManager.dsSVBiCamThi(maLHP);
    lxw_workbook  *wb = workbook_new(filePath.string().c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Danh Sách Cấm Thi");

    // Tieu de merged
    lxw_format *fTitle = workbook_add_format(wb);
    format_set_bold(fTitle); format_set_font_size(fTitle, 14);
    format_set_bg_color(fTitle, 0xC00000); // do dam
    format_set_font_color(fTitle, LXW_COLOR_WHITE);
    format_set_align(fTitle, LXW_ALIGN_CENTER);
    format_set_align(fTitle, LXW_ALIGN_VERTICAL_CENTER);
    worksheet_merge_range(ws, 0, 0, 0, 4,
        ("DANH SÁCH SINH VIÊN BỊ CẤM THI - " + maLHP).c_str(), fTitle);
    worksheet_set_row(ws, 0, 28, nullptr);

    lxw_format *fHeader = workbook_add_format(wb);
    format_set_bold(fHeader); format_set_bg_color(fHeader, 0xC00000);
    format_set_font_color(fHeader, LXW_COLOR_WHITE);
    format_set_border(fHeader, LXW_BORDER_THIN);
    format_set_align(fHeader, LXW_ALIGN_CENTER);

    lxw_format *fRow = workbook_add_format(wb);
    format_set_bg_color(fRow, 0xFFCCCC);
    format_set_border(fRow, LXW_BORDER_THIN);

    lxw_format *fPct = workbook_add_format(wb);
    format_set_bg_color(fPct, 0xFFCCCC);
    format_set_border(fPct, LXW_BORDER_THIN);
    format_set_num_format(fPct, "0.0%");

    const std::vector<std::string> hdrs =
        {"STT", "Mã SV", "Họ Tên", "Tiết Vắng", "% Vắng"};
    for (int c = 0; c < 5; ++c)
        worksheet_write_string(ws, 1, (lxw_col_t)c, hdrs[c].c_str(), fHeader);
    worksheet_set_column(ws, 0, 0, 6,  nullptr);
    worksheet_set_column(ws, 1, 1, 14, nullptr);
    worksheet_set_column(ws, 2, 2, 30, nullptr);

    for (int i = 0; i < (int)dsSV.size(); ++i) {
        const auto &sv = dsSV[i];
        lxw_row_t r = (lxw_row_t)(i + 2);
        worksheet_write_number(ws, r, 0, i + 1,          fRow);
        worksheet_write_string(ws, r, 1, sv.maSV.c_str(),  fRow);
        worksheet_write_string(ws, r, 2, sv.tenSV.c_str(), fRow);
        worksheet_write_number(ws, r, 3, sv.soTietVang,    fRow);
        worksheet_write_number(ws, r, 4, sv.tyLeVang,      fPct);
    }
    workbook_close(wb);
}

void BaoCaoManager::xuatTongQuanXLSX(const std::filesystem::path &filePath) {
    auto dsLop = _tkManager.thongKeTatCaLop();
    lxw_workbook  *wb = workbook_new(filePath.string().c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Tổng Quan Hệ Thống");

    lxw_format *fHeader = workbook_add_format(wb);
    format_set_bold(fHeader); format_set_bg_color(fHeader, 0x1F4E79);
    format_set_font_color(fHeader, LXW_COLOR_WHITE);
    format_set_border(fHeader, LXW_BORDER_THIN);
    format_set_align(fHeader, LXW_ALIGN_CENTER);

    lxw_format *fNormal  = workbook_add_format(wb);
    format_set_border(fNormal, LXW_BORDER_THIN);
    lxw_format *fWarnRow = workbook_add_format(wb);
    format_set_bg_color(fWarnRow, 0xFFCC99);
    format_set_border(fWarnRow, LXW_BORDER_THIN);
    lxw_format *fPct = workbook_add_format(wb);
    format_set_border(fPct, LXW_BORDER_THIN);
    format_set_num_format(fPct, "0.0%");
    lxw_format *fPctW = workbook_add_format(wb);
    format_set_bg_color(fPctW, 0xFFCC99);
    format_set_border(fPctW, LXW_BORDER_THIN);
    format_set_num_format(fPctW, "0.0%");

    writeHeaderRow(ws, fHeader, {
        "Mã LHP", "Tên LHP", "Số SV",
        "Số Buổi", "Tiết Đã Học", "% Vắng TB", "SV Cấm Thi"
    });
    worksheet_set_column(ws, 0, 0, 12, nullptr);
    worksheet_set_column(ws, 1, 1, 35, nullptr);

    for (int i = 0; i < (int)dsLop.size(); ++i) {
        const auto &lop = dsLop[i];
        lxw_row_t row = (lxw_row_t)(i + 1);
        bool warn = lop.tyLeVangTrungBinh > 0.3;
        lxw_format *fR = warn ? fWarnRow : fNormal;
        lxw_format *fP = warn ? fPctW    : fPct;
        worksheet_write_string(ws, row, 0, lop.maLHP.c_str(),  fR);
        worksheet_write_string(ws, row, 1, lop.tenLHP.c_str(), fR);
        worksheet_write_number(ws, row, 2, lop.soSinhVien,     fR);
        worksheet_write_number(ws, row, 3, lop.soBuoiDaHoc,   fR);
        worksheet_write_number(ws, row, 4, lop.soTietDaHoc,   fR);
        worksheet_write_number(ws, row, 5, lop.tyLeVangTrungBinh, fP);
        worksheet_write_number(ws, row, 6, lop.soSVBiCamThi,  fR);
    }
    workbook_close(wb);
}

void BaoCaoManager::xuatToanDienXLSX(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV   = _tkManager.thongKeToanLop(maLHP);
    auto dsBuoi = _tkManager.thongKeTatCaBuoi(maLHP);
    auto dsCT   = _tkManager.dsSVBiCamThi(maLHP);
    auto tkLop  = _tkManager.thongKeLop(maLHP);
    auto buoiMax = _tkManager.buoiVangCaoNhat(maLHP);
    std::size_t maxIdx = buoiMax.has_value() ? buoiMax->buoiIndex : SIZE_MAX;

    lxw_workbook *wb = workbook_new(filePath.string().c_str());

    // --- Formats chung ---
    auto mkHeader = [&](uint32_t bg) {
        lxw_format *f = workbook_add_format(wb);
        format_set_bold(f); format_set_bg_color(f, bg);
        format_set_font_color(f, LXW_COLOR_WHITE);
        format_set_border(f, LXW_BORDER_THIN);
        format_set_align(f, LXW_ALIGN_CENTER);
        return f;
    };
    auto mkCell = [&](uint32_t bg = 0xFFFFFF) {
        lxw_format *f = workbook_add_format(wb);
        if (bg != 0xFFFFFF) format_set_bg_color(f, bg);
        format_set_border(f, LXW_BORDER_THIN);
        return f;
    };
    auto mkPct = [&](uint32_t bg = 0xFFFFFF) {
        lxw_format *f = workbook_add_format(wb);
        if (bg != 0xFFFFFF) format_set_bg_color(f, bg);
        format_set_border(f, LXW_BORDER_THIN);
        format_set_num_format(f, "0.0%");
        return f;
    };
    lxw_format *fLabel = workbook_add_format(wb);
    format_set_bold(fLabel); format_set_bg_color(fLabel, 0xD6E4F0);
    format_set_border(fLabel, LXW_BORDER_THIN);

    lxw_format *fTitle = workbook_add_format(wb);
    format_set_bold(fTitle); format_set_font_size(fTitle, 14);
    format_set_bg_color(fTitle, 0x1F4E79);
    format_set_font_color(fTitle, LXW_COLOR_WHITE);
    format_set_align(fTitle, LXW_ALIGN_CENTER);
    format_set_align(fTitle, LXW_ALIGN_VERTICAL_CENTER);

    // ---- SHEET 1: Tong Quan LHP ----
    {
        lxw_worksheet *ws = workbook_add_worksheet(wb, "Tổng Quan LHP");
        worksheet_merge_range(ws, 0, 0, 0, 3,
            ("BÁO CÁO ĐIỂM DANH - " + maLHP + " - " + tkLop.tenLHP).c_str(), fTitle);
        worksheet_set_row(ws, 0, 30, nullptr);
        worksheet_set_column(ws, 0, 0, 22, nullptr);
        worksheet_set_column(ws, 1, 3, 25, nullptr);

        lxw_format *fNorm = mkCell();
        struct Row { const char* label; std::string val; };
        std::vector<Row> rows = {
            {"Mã lớp học phần",  maLHP},
            {"Tên lớp học phần", tkLop.tenLHP},
            {"Số sinh viên",     std::to_string(tkLop.soSinhVien)},
            {"Số buổi đã học",   std::to_string(tkLop.soBuoiDaHoc)},
            {"Số tiết đã học",   std::to_string(tkLop.soTietDaHoc)},
            {"SV bị cấm thi",    std::to_string(tkLop.soSVBiCamThi)},
        };
        for (int r = 0; r < (int)rows.size(); ++r) {
            worksheet_write_string(ws, (lxw_row_t)(r+2), 0, rows[r].label, fLabel);
            worksheet_write_string(ws, (lxw_row_t)(r+2), 1, rows[r].val.c_str(), fNorm);
        }
        // % vang TB
        lxw_format *fPLabel = workbook_add_format(wb);
        format_set_bold(fPLabel); format_set_bg_color(fPLabel, 0xD6E4F0);
        format_set_border(fPLabel, LXW_BORDER_THIN);
        lxw_format *fP = mkPct();
        worksheet_write_string(ws, (lxw_row_t)(rows.size()+2), 0,
                               "% vắng trung bình", fLabel);
        worksheet_write_number(ws, (lxw_row_t)(rows.size()+2), 1,
                               tkLop.tyLeVangTrungBinh, fP);
    }

    // ---- SHEET 2: Diem Danh Buoi ----
    {
        lxw_worksheet *ws = workbook_add_worksheet(wb, "Điểm Danh Buổi");
        lxw_format *fH  = mkHeader(0x2E75B6);
        lxw_format *fN  = mkCell();
        lxw_format *fHL = mkCell(0xFF9999);
        lxw_format *fP  = mkPct();
        lxw_format *fPHL= mkPct(0xFF9999);
        writeHeaderRow(ws, fH, {"Buổi","Ngày","Ca","Số Tiết","Có Mặt","Vắng","Muộn","% Có Mặt"});
        worksheet_set_column(ws, 1, 1, 14, nullptr);
        for (int i = 0; i < (int)dsBuoi.size(); ++i) {
            const auto &b = dsBuoi[i];
            lxw_row_t row = (lxw_row_t)(i+1);
            bool isMax = (b.buoiIndex == maxIdx);
            auto fR = isMax ? fHL : fN;
            auto fPR = isMax ? fPHL : fP;
            worksheet_write_number(ws, row, 0, (int)b.buoiIndex+1, fR);
            worksheet_write_string(ws, row, 1, b.ngay.c_str(), fR);
            worksheet_write_string(ws, row, 2, b.ca.c_str(),   fR);
            worksheet_write_number(ws, row, 3, b.soTiet,  fR);
            worksheet_write_number(ws, row, 4, b.soCoMat, fR);
            worksheet_write_number(ws, row, 5, b.soVang,  fR);
            worksheet_write_number(ws, row, 6, b.soMuon,  fR);
            worksheet_write_number(ws, row, 7, b.tyLeCoMat, fPR);
        }
    }

    // ---- SHEET 3: Danh Sach SV ----
    {
        lxw_worksheet *ws = workbook_add_worksheet(wb, "Danh Sách SV");
        lxw_format *fH      = mkHeader(0x2E75B6);
        lxw_format *fOk     = mkCell();
        lxw_format *fWarn   = mkCell(0xFFFF99);
        lxw_format *fDanger = mkCell(0xFFCC99);
        lxw_format *fCT     = workbook_add_format(wb);
        format_set_bg_color(fCT, 0xFF0000); format_set_font_color(fCT, LXW_COLOR_WHITE);
        format_set_bold(fCT); format_set_border(fCT, LXW_BORDER_THIN);
        lxw_format *fPct    = mkPct();
        writeHeaderRow(ws, fH, {"STT","Mã SV","Họ Tên","Tiết Vắng","Tiết Muộn","Tiết Có Mặt","% Vắng","Trạng Thái"});
        worksheet_set_column(ws, 0, 0, 6,  nullptr);
        worksheet_set_column(ws, 1, 1, 12, nullptr);
        worksheet_set_column(ws, 2, 2, 30, nullptr);
        for (int i = 0; i < (int)dsSV.size(); ++i) {
            const auto &sv = dsSV[i];
            lxw_row_t row = (lxw_row_t)(i+1);
            lxw_format *fR = sv.biCamThi       ? fCT
                           : sv.tyLeVang > 0.8 ? fDanger
                           : sv.tyLeVang > 0.5 ? fWarn : fOk;
            std::string tt = sv.biCamThi       ? "CẤM THI"
                           : sv.tyLeVang > 0.8 ? "Nguy hiểm"
                           : sv.tyLeVang > 0.5 ? "Cần chú ý" : "Bình thường";
            worksheet_write_number(ws, row, 0, i+1,              fR);
            worksheet_write_string(ws, row, 1, sv.maSV.c_str(),  fR);
            worksheet_write_string(ws, row, 2, sv.tenSV.c_str(), fR);
            worksheet_write_number(ws, row, 3, sv.soTietVang,    fR);
            worksheet_write_number(ws, row, 4, sv.soTietMuon,    fR);
            worksheet_write_number(ws, row, 5, sv.soTietCoMat,   fR);
            worksheet_write_number(ws, row, 6, sv.tyLeVang,      fPct);
            worksheet_write_string(ws, row, 7, tt.c_str(),       fR);
        }
    }

    // ---- SHEET 4: Cam Thi ----
    {
        lxw_worksheet *ws = workbook_add_worksheet(wb, "Danh Sách Cấm Thi");
        lxw_format *fT2 = workbook_add_format(wb);
        format_set_bold(fT2); format_set_bg_color(fT2, 0xC00000);
        format_set_font_color(fT2, LXW_COLOR_WHITE);
        format_set_align(fT2, LXW_ALIGN_CENTER);
        format_set_align(fT2, LXW_ALIGN_VERTICAL_CENTER);
        worksheet_merge_range(ws, 0, 0, 0, 4,
            ("DANH SÁCH SINH VIÊN BỊ CẤM THI - " + maLHP + " - " + tkLop.tenLHP).c_str(), fT2);
        worksheet_set_row(ws, 0, 28, nullptr);

        lxw_format *fH   = mkHeader(0xC00000);
        lxw_format *fRow = mkCell(0xFFCCCC);
        lxw_format *fP   = mkPct(0xFFCCCC);
        const char *hdrs[] = {"STT","Mã SV","Họ Tên","Tiết Vắng","% Vắng"};
        for (int c = 0; c < 5; ++c)
            worksheet_write_string(ws, 1, (lxw_col_t)c, hdrs[c], fH);
        worksheet_set_column(ws, 0, 0, 6,  nullptr);
        worksheet_set_column(ws, 1, 1, 14, nullptr);
        worksheet_set_column(ws, 2, 2, 30, nullptr);
        for (int i = 0; i < (int)dsCT.size(); ++i) {
            const auto &sv = dsCT[i];
            lxw_row_t r = (lxw_row_t)(i+2);
            worksheet_write_number(ws, r, 0, i+1,              fRow);
            worksheet_write_string(ws, r, 1, sv.maSV.c_str(),  fRow);
            worksheet_write_string(ws, r, 2, sv.tenSV.c_str(), fRow);
            worksheet_write_number(ws, r, 3, sv.soTietVang,    fRow);
            worksheet_write_number(ws, r, 4, sv.tyLeVang,      fP);
        }
    }

    workbook_close(wb);
}
