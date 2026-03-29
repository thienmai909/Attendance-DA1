#include <BaoCaoManager.hpp>

BaoCaoManager::BaoCaoManager(
    ThongKeManager &tkManager,
    std::filesystem::path outputDir
) : _tkManager(tkManager)
  , _outputDir(std::move(outputDir))
{}

void BaoCaoManager::ensureOutputDir()
{
    if (!std::filesystem::exists(_outputDir))
        std::filesystem::create_directories(_outputDir);
}

std::filesystem::path BaoCaoManager::taoTenFile(
    const std::string &maLHP,
    const std::string &loai,
    DinhDangXuat dinhDang
) {
    std::string ext = (dinhDang == DinhDangXuat::CSV) ? ".csv" : ".json";
    std::string tenFile = maLHP + "_" + loai + ext;
    return _outputDir / tenFile;
}

std::filesystem::path BaoCaoManager::taoTenFile(
    const std::string &loai,
    DinhDangXuat dinhDang
) {
    std::string ext = (dinhDang == DinhDangXuat::CSV) ? ".csv" : ".json";
    return _outputDir / (loai + ext);
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
