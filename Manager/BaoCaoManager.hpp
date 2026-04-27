#pragma once

#include <ThongKeManager.hpp>
#include <Utility.hpp>

#include <string>
#include <filesystem>

enum class DinhDangXuat {
    CSV, JSON, XLSX
};

class BaoCaoManager {
    ThongKeManager& _tkManager;
    std::filesystem::path _outputDir;

public:
    BaoCaoManager(
        ThongKeManager& tkManager,
        std::filesystem::path outputDir = "output"
    );

    void xuatBaoCaoBuoi(
        const std::string& maLHP,
        std::size_t buoiIndex,
        DinhDangXuat dinhDang = DinhDangXuat::XLSX
    );
    void xuatBaoCaoTatCaBuoi(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::XLSX
    );
    void xuatBaoCaoSinhVien(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::XLSX
    );
    void xuatDanhSachCamThi(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::XLSX
    );
    void xuatBaoCaoTongQuan(
        DinhDangXuat dinhDang = DinhDangXuat::XLSX
    );
    // Xuat 1 file XLSX toan dien (4 sheet) cho 1 lop
    std::string xuatBaoCaoToanDien(const std::string& maLHP);

private:
    void ensureOutputDir();
    std::string taoTimestamp() const;

    std::filesystem::path taoTenFile(
        const std::string& maLHP,
        const std::string& loai,
        DinhDangXuat dinhDang
    );
    std::filesystem::path taoTenFile(
        const std::string& loai,
        DinhDangXuat dinhDang
    );
    std::filesystem::path taoTenFileXLSX(
        const std::string& maLHP,
        const std::string& loai
    );

    // CSV writers
    void xuatBuoiCSV(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::filesystem::path& filePath
    );
    void xuatTatCaBuoiCSV(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatSinhVienCSV(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatCamThiCSV(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatTongQuanCSV(
        const std::filesystem::path& filePath
    );

    void xuatBuoiJSON(
        const std::string& maLHP,
        std::size_t buoiIndex,
        const std::filesystem::path& filePath
    );
    void xuatTatCaBuoiJSON(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatSinhVienJSON(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatCamThiJSON(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatTongQuanJSON(
        const std::filesystem::path& filePath
    );

    // XLSX writers
    void xuatToanDienXLSX(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatSinhVienXLSX(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatTatCaBuoiXLSX(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatCamThiXLSX(
        const std::string& maLHP,
        const std::filesystem::path& filePath
    );
    void xuatTongQuanXLSX(
        const std::filesystem::path& filePath
    );
};