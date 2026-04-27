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

// Viet header row
static void writeHeaderRow(lxw_worksheet* ws, lxw_format* fmt,
                           const std::vector<std::string>& headers,
                           double colWidth = 20.0) {
    for (int c = 0; c < (int)headers.size(); ++c) {
        worksheet_write_string(ws, 0, (lxw_col_t)c,
                               headers[c].c_str(), fmt);
        worksheet_set_column(ws, (lxw_col_t)c, (lxw_col_t)c, colWidth, nullptr);
    }
}

// Viet block thong tin lop (dung cho sheet standalone)
static lxw_row_t writeInfoBlock(lxw_worksheet* ws, lxw_workbook* wb,
                                const LopHocPhan& lhp,
                                const std::string& tenGV,
                                const std::string& timestamp,
                                int nCT, int soSV) {
    lxw_format *fTitle = workbook_add_format(wb);
    format_set_bold(fTitle); format_set_font_size(fTitle, 13);
    format_set_bg_color(fTitle, 0x1F4E79);
    format_set_font_color(fTitle, LXW_COLOR_WHITE);
    format_set_align(fTitle, LXW_ALIGN_CENTER);
    format_set_align(fTitle, LXW_ALIGN_VERTICAL_CENTER);

    lxw_format *fLabel = workbook_add_format(wb);
    format_set_bold(fLabel); format_set_bg_color(fLabel, 0xD6E4F0);
    format_set_border(fLabel, LXW_BORDER_THIN);

    lxw_format *fVal = workbook_add_format(wb);
    format_set_border(fVal, LXW_BORDER_THIN);

    lxw_format *fPct = workbook_add_format(wb);
    format_set_border(fPct, LXW_BORDER_THIN);
    format_set_num_format(fPct, "0.0%");

    lxw_format *fWarn = workbook_add_format(wb);
    format_set_border(fWarn, LXW_BORDER_THIN);
    format_set_font_color(fWarn, 0xC00000);
    format_set_bold(fWarn);

    int nguongPct = (int)(lhp.getNguongCamThi() * 100);
    int maxVang   = (int)(lhp.getTongSoTiet() * lhp.getNguongCamThi());
    int pctTienDo = lhp.getTongSoTiet() > 0
        ? (int)(100.0 * lhp.getSoTietDaHoc() / lhp.getTongSoTiet()) : 0;
    std::string tenPhong = lhp.getTenPhongHoc().empty() ? "(Chưa có)" : lhp.getTenPhongHoc();

    // Row 0: Title merged
    int cols = 6;
    worksheet_merge_range(ws, 0, 0, 0, cols - 1,
        ("BÁO CÁO ĐIỂM DANH - " + lhp.getMaLHP() + " - " + lhp.getTenLHP()).c_str(),
        fTitle);
    worksheet_set_row(ws, 0, 28, nullptr);
    worksheet_set_column(ws, 0, 0, 22, nullptr);
    worksheet_set_column(ws, 1, 1, 30, nullptr);
    worksheet_set_column(ws, 2, 2, 18, nullptr);
    worksheet_set_column(ws, 3, 3, 18, nullptr);

    // Rows 2-3: 2 cot thong tin
    struct KV { const char* k; std::string v; bool isWarn; };
    std::vector<KV> left = {
        {"Mã lớp học phần:",  lhp.getMaLHP(),           false},
        {"Tên lớp học phần:", lhp.getTenLHP(),           false},
        {"Giảng viên:",       tenGV,                     false},
        {"Phòng học:",        tenPhong,                  false},
        {"Học kỳ:",           lhp.getHocKiStr(),         false},
    };
    std::vector<KV> right = {
        {"Ngưỡng cấm thi:",   std::to_string(nguongPct) + "% (max " + std::to_string(maxVang) + " tiết)", false},
        {"Tổng số tiết:",     std::to_string(lhp.getTongSoTiet()) + " tiết",                              false},
        {"Tiến độ học tập:",  std::to_string(lhp.getSoTietDaHoc()) + "/" + std::to_string(lhp.getTongSoTiet())
                              + " tiết (" + std::to_string(pctTienDo) + "%)",                              false},
        {"Số sinh viên:",     std::to_string(soSV) + " SV",                                               false},
        {"SV bị cấm thi:",   std::to_string(nCT) + " SV",                                                 nCT > 0},
    };
    int maxR = (int)std::max(left.size(), right.size());
    for (int r = 0; r < maxR; ++r) {
        lxw_row_t row = (lxw_row_t)(r + 2);
        if (r < (int)left.size()) {
            worksheet_write_string(ws, row, 0, left[r].k,        fLabel);
            worksheet_write_string(ws, row, 1, left[r].v.c_str(), left[r].isWarn ? fWarn : fVal);
        }
        if (r < (int)right.size()) {
            worksheet_write_string(ws, row, 2, right[r].k,         fLabel);
            worksheet_write_string(ws, row, 3, right[r].v.c_str(), right[r].isWarn ? fWarn : fVal);
        }
    }

    // Ngay xuat
    lxw_row_t rowNgay = (lxw_row_t)(maxR + 3);
    lxw_format *fDim = workbook_add_format(wb);
    format_set_italic(fDim); format_set_font_color(fDim, 0x808080);
    worksheet_write_string(ws, rowNgay, 0, "Ngày xuất báo cáo:", fLabel);
    worksheet_write_string(ws, rowNgay, 1, timestamp.c_str(), fDim);

    return rowNgay + 2; // row bat dau du lieu
}

void BaoCaoManager::xuatSinhVienXLSX(
    const std::string &maLHP,
    const std::filesystem::path &filePath
) {
    auto dsSV     = _tkManager.thongKeToanLop(maLHP);
    const auto& lhp = _tkManager.getLHPRef(maLHP);
    int maxVang   = (int)(lhp.getTongSoTiet() * lhp.getNguongCamThi());
    int nCT       = 0;
    for (const auto& sv : dsSV) if (sv.biCamThi) ++nCT;

    lxw_workbook  *wb = workbook_new(filePath.string().c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Danh Sách SV");

    lxw_row_t startRow = writeInfoBlock(ws, wb, lhp, "", taoTimestamp(), nCT, (int)dsSV.size());

    lxw_format *fHeader = workbook_add_format(wb);
    format_set_bold(fHeader); format_set_bg_color(fHeader, 0x2E75B6);
    format_set_font_color(fHeader, LXW_COLOR_WHITE);
    format_set_border(fHeader, LXW_BORDER_THIN); format_set_align(fHeader, LXW_ALIGN_CENTER);

    auto mkFmt = [&](uint32_t bg = 0xFFFFFF) {
        lxw_format *f = workbook_add_format(wb);
        if (bg != 0xFFFFFF) format_set_bg_color(f, bg);
        format_set_border(f, LXW_BORDER_THIN); return f;
    };
    auto mkPct = [&](uint32_t bg = 0xFFFFFF) {
        lxw_format *f = workbook_add_format(wb);
        if (bg != 0xFFFFFF) format_set_bg_color(f, bg);
        format_set_border(f, LXW_BORDER_THIN);
        format_set_num_format(f, "0.0%"); return f;
    };
    lxw_format *fOk     = mkFmt();
    lxw_format *fWarn   = mkFmt(0xFFFF99);
    lxw_format *fDanger = mkFmt(0xFFCC99);
    lxw_format *fCT     = workbook_add_format(wb);
    format_set_bg_color(fCT, 0xFF0000); format_set_font_color(fCT, LXW_COLOR_WHITE);
    format_set_bold(fCT); format_set_border(fCT, LXW_BORDER_THIN);
    lxw_format *fCon    = workbook_add_format(wb);
    format_set_bg_color(fCon, 0xE2EFDA); format_set_border(fCon, LXW_BORDER_THIN); // xanh la nhat
    lxw_format *fConBad = workbook_add_format(wb);
    format_set_bg_color(fConBad, 0xFFCCCC); format_set_border(fConBad, LXW_BORDER_THIN); // do nhat

    // Header
    const std::vector<std::string> hdrs = {
        "STT", "Mã SV", "Họ Tên", "Tiết Vắng", "Tiết Muộn",
        "Tiết Có Mặt", "% Vắng", "Còn được vắng", "Trạng Thái"
    };
    for (int c = 0; c < (int)hdrs.size(); ++c)
        worksheet_write_string(ws, startRow, (lxw_col_t)c, hdrs[c].c_str(), fHeader);
    worksheet_set_column(ws, 0, 0, 6,  nullptr);
    worksheet_set_column(ws, 1, 1, 12, nullptr);
    worksheet_set_column(ws, 2, 2, 30, nullptr);
    worksheet_set_column(ws, 6, 6, 10, nullptr);
    worksheet_set_column(ws, 7, 7, 16, nullptr);
    worksheet_set_column(ws, 8, 8, 14, nullptr);
    lxw_format *fPct = mkPct();

    int tongVang = 0;
    for (int i = 0; i < (int)dsSV.size(); ++i) {
        const auto& sv = dsSV[i];
        lxw_row_t row  = startRow + 1 + (lxw_row_t)i;
        int con        = maxVang - sv.soTietVang;
        tongVang      += sv.soTietVang;
        lxw_format *fR = sv.biCamThi       ? fCT
                       : sv.tyLeVang > 0.8 ? fDanger
                       : sv.tyLeVang > 0.5 ? fWarn : fOk;
        std::string tt = sv.biCamThi       ? "CẤM THI"
                       : sv.tyLeVang > 0.8 ? "Nguy hiểm"
                       : sv.tyLeVang > 0.5 ? "Cần chú ý" : "Bình thường";
        std::string conStr = con > 0
            ? "+" + std::to_string(con) + " tiết"
            : "Vượt " + std::to_string(-con) + " tiết";

        worksheet_write_number(ws, row, 0, i + 1,              fR);
        worksheet_write_string(ws, row, 1, sv.maSV.c_str(),    fR);
        worksheet_write_string(ws, row, 2, sv.tenSV.c_str(),   fR);
        worksheet_write_number(ws, row, 3, sv.soTietVang,      fR);
        worksheet_write_number(ws, row, 4, sv.soTietMuon,      fR);
        worksheet_write_number(ws, row, 5, sv.soTietCoMat,     fR);
        worksheet_write_number(ws, row, 6, sv.tyLeVang,        fPct);
        worksheet_write_string(ws, row, 7, conStr.c_str(),     con > 0 ? fCon : fConBad);
        worksheet_write_string(ws, row, 8, tt.c_str(),         fR);
    }

    // Dong tong ket
    lxw_row_t rowSum = startRow + 1 + (lxw_row_t)dsSV.size();
    lxw_format *fSum = workbook_add_format(wb);
    format_set_bold(fSum); format_set_bg_color(fSum, 0xD6E4F0); format_set_border(fSum, LXW_BORDER_THIN);
    worksheet_write_string(ws, rowSum, 0, "TỔNG KẾT", fSum);
    worksheet_write_string(ws, rowSum, 1, (std::to_string(dsSV.size()) + " SV").c_str(), fSum);
    worksheet_write_string(ws, rowSum, 3, (std::to_string(tongVang) + " tiết").c_str(), fSum);
    worksheet_write_string(ws, rowSum, 8, ("Cấm thi: " + std::to_string(nCT) + " SV").c_str(), fSum);

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
    const auto& lhp = _tkManager.getLHPRef(maLHP);
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

        // Lay ten GV
        std::string tenGVStr = "(Chưa phân công)";
        // Note: BaoCaoManager khong co reference GVManager - dung maGV
        if (!lhp.getMaGV().empty()) tenGVStr = lhp.getMaGV();

        // Dung writeInfoBlock cho nhat quan
        int nCTSheet1 = (int)dsCT.size();
        int soSVSheet1 = (int)dsSV.size();
        // Viet thu cong vi co them du lieu tong quan
        int nguongPct = (int)(lhp.getNguongCamThi() * 100);
        int maxVangLop = (int)(lhp.getTongSoTiet() * lhp.getNguongCamThi());
        int pctTienDo  = lhp.getTongSoTiet() > 0
            ? (int)(100.0 * lhp.getSoTietDaHoc() / lhp.getTongSoTiet()) : 0;
        std::string tenPhong = lhp.getTenPhongHoc().empty() ? "(Chưa có)" : lhp.getTenPhongHoc();

        // Dem nhom SV
        int svNguy = 0, svChuY = 0, svOk = 0;
        for (const auto& sv : dsSV) {
            if (sv.biCamThi) ; // da co nCTSheet1
            else if (sv.tyLeVang > 0.8) ++svNguy;
            else if (sv.tyLeVang > 0.5) ++svChuY;
            else ++svOk;
        }

        worksheet_set_column(ws, 0, 0, 26, nullptr);
        worksheet_set_column(ws, 1, 1, 32, nullptr);
        worksheet_set_column(ws, 2, 2, 24, nullptr);
        worksheet_set_column(ws, 3, 3, 24, nullptr);

        // Title row
        worksheet_merge_range(ws, 0, 0, 0, 3,
            ("BÁO CÁO ĐIỂM DANH - " + maLHP + " - " + tkLop.tenLHP).c_str(), fTitle);
        worksheet_set_row(ws, 0, 30, nullptr);

        // Block thong tin (2 cot x 5 hang, bat dau tu row 2)
        struct KV2 { const char* k; std::string v; bool warn; };
        lxw_format *fWarnCell = workbook_add_format(wb);
        format_set_border(fWarnCell, LXW_BORDER_THIN);
        format_set_font_color(fWarnCell, 0xC00000); format_set_bold(fWarnCell);

        std::vector<KV2> left2 = {
            {"Mã lớp học phần:",  maLHP,               false},
            {"Tên lớp học phần:", tkLop.tenLHP,         false},
            {"Giảng viên (Mã):",  tenGVStr,             false},
            {"Phòng học:",        tenPhong,             false},
            {"Học kỳ:",           lhp.getHocKiStr(),   false},
        };
        std::vector<KV2> right2 = {
            {"Ngưỡng cấm thi:",   std::to_string(nguongPct) + "% (tối đa " + std::to_string(maxVangLop) + " tiết)", false},
            {"Tổng số tiết:",     std::to_string(lhp.getTongSoTiet()) + " tiết", false},
            {"Tiến độ học tập:",  std::to_string(lhp.getSoTietDaHoc()) + "/" + std::to_string(lhp.getTongSoTiet())
                                  + " tiết (" + std::to_string(pctTienDo) + "%)", false},
            {"Số SV ghi danh:",   std::to_string(soSVSheet1) + " SV", false},
            {"Số buổi đã học:",   std::to_string(tkLop.soBuoiDaHoc) + " buổi / " + std::to_string(tkLop.soTietDaHoc) + " tiết", false},
        };
        int maxR2 = (int)std::max(left2.size(), right2.size());
        for (int r = 0; r < maxR2; ++r) {
            lxw_row_t row = (lxw_row_t)(r + 2);
            if (r < (int)left2.size()) {
                worksheet_write_string(ws, row, 0, left2[r].k,         fLabel);
                worksheet_write_string(ws, row, 1, left2[r].v.c_str(), mkCell());
            }
            if (r < (int)right2.size()) {
                worksheet_write_string(ws, row, 2, right2[r].k,         fLabel);
                worksheet_write_string(ws, row, 3, right2[r].v.c_str(), right2[r].warn ? fWarnCell : mkCell());
            }
        }

        // Hang phan cach
        lxw_row_t rowSep = (lxw_row_t)(maxR2 + 3);
        lxw_format *fSub = workbook_add_format(wb);
        format_set_bold(fSub); format_set_bg_color(fSub, 0x2E75B6);
        format_set_font_color(fSub, LXW_COLOR_WHITE); format_set_border(fSub, LXW_BORDER_THIN);
        worksheet_merge_range(ws, rowSep, 0, rowSep, 3, "THỐNG KÊ TÌNH TRẠNG SINH VIÊN", fSub);
        worksheet_set_row(ws, rowSep, 20, nullptr);

        // Phan nhom SV
        struct NhomInfo { std::string ten; int so; uint32_t bg; uint32_t fg; };
        std::vector<NhomInfo> nhoms = {
            {"Bình thường (vắng < 50% ngưỡng)",  svOk,        0xE2EFDA, 0x375623},
            {"Cần chú ý (50%-80% ngưỡng)",       svChuY,      0xFFFF99, 0x7D6608},
            {"Nguy hiểm (>80% ngưỡng)",          svNguy,      0xFFCC99, 0x843C0C},
            {"Bị cấm thi (≥ ngưỡng)",           nCTSheet1,   0xFF0000, 0xFFFFFF},
        };
        lxw_format *fTenNhom = workbook_add_format(wb);
        format_set_border(fTenNhom, LXW_BORDER_THIN);
        for (int n = 0; n < (int)nhoms.size(); ++n) {
            lxw_row_t row = (lxw_row_t)(rowSep + 1 + n);
            lxw_format *fNhom = workbook_add_format(wb);
            format_set_border(fNhom, LXW_BORDER_THIN);
            format_set_bg_color(fNhom, nhoms[n].bg);
            format_set_font_color(fNhom, nhoms[n].fg);
            format_set_bold(fNhom);
            worksheet_write_string(ws, row, 0, nhoms[n].ten.c_str(), fNhom);
            worksheet_write_number(ws, row, 1, nhoms[n].so, fNhom);
            std::string pctStr = soSVSheet1 > 0
                ? std::to_string((int)(100.0 * nhoms[n].so / soSVSheet1)) + "% số SV"
                : "-";
            worksheet_write_string(ws, row, 2, pctStr.c_str(), fNhom);
        }

        // % vang TB
        lxw_row_t rowPct = (lxw_row_t)(rowSep + 1 + nhoms.size() + 1);
        worksheet_write_string(ws, rowPct, 0, "% Vắng trung bình toàn lớp:", fLabel);
        lxw_format *fPctVal = mkPct();
        worksheet_write_number(ws, rowPct, 1, tkLop.tyLeVangTrungBinh, fPctVal);

        // Ngay xuat
        lxw_row_t rowNgay2 = (lxw_row_t)(rowPct + 2);
        lxw_format *fDim2 = workbook_add_format(wb);
        format_set_italic(fDim2); format_set_font_color(fDim2, 0x808080);
        worksheet_write_string(ws, rowNgay2, 0, "Ngày xuất báo cáo:", fLabel);
        worksheet_write_string(ws, rowNgay2, 1, taoTimestamp().c_str(), fDim2);
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
        int tongTiet=0, tongVang=0, tongMuon=0, tongCoMat=0;
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
            tongTiet  += b.soTiet;
            tongVang  += b.soVang;
            tongMuon  += b.soMuon;
            tongCoMat += b.soCoMat;
        }
        // Dong tong ket
        lxw_row_t rowSum = (lxw_row_t)(dsBuoi.size() + 1);
        lxw_format *fSum2 = workbook_add_format(wb);
        format_set_bold(fSum2); format_set_bg_color(fSum2, 0xD6E4F0);
        format_set_border(fSum2, LXW_BORDER_THIN);
        lxw_format *fPctSum = mkPct(0xD6E4F0);
        worksheet_write_string(ws, rowSum, 0, "TỔNG / TB", fSum2);
        worksheet_write_string(ws, rowSum, 1, (std::to_string(dsBuoi.size()) + " buổi").c_str(), fSum2);
        worksheet_write_string(ws, rowSum, 2, "", fSum2);
        worksheet_write_number(ws, rowSum, 3, tongTiet,  fSum2);
        worksheet_write_number(ws, rowSum, 4, tongCoMat, fSum2);
        worksheet_write_number(ws, rowSum, 5, tongVang,  fSum2);
        worksheet_write_number(ws, rowSum, 6, tongMuon,  fSum2);
        double tbCoMat = tongTiet > 0 ? (double)tongCoMat / tongTiet : 0.0;
        worksheet_write_number(ws, rowSum, 7, tbCoMat,   fPctSum);
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
