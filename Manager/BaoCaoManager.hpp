#pragma once

#include <ThongKeManager.hpp>
#include <Utility.hpp>

#include <string>
#include <filesystem>

enum class DinhDangXuat {
    CSV, JSON
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
        const std::string&maLHP,
        std::size_t buoiIndex,
        DinhDangXuat dinhDang = DinhDangXuat::CSV
    );
    void xuatBaoCaoTatCaBuoi(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::CSV
    );
    void xuatBaoCaoSinhVien(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::CSV
    );
    void xuatDanhSachCamThi(
        const std::string& maLHP,
        DinhDangXuat dinhDang = DinhDangXuat::CSV
    );
    void xuatBaoCaoTongQuan(
        DinhDangXuat dinhDang = DinhDangXuat::CSV
    );

private:
    void ensureOutputDir();

    std::filesystem::path taoTenFile(
        const std::string& maLHP,
        const std::string& loai,
        DinhDangXuat dinhDang
    );
    std::filesystem::path taoTenFile(
        const std::string& loai,
        DinhDangXuat dinhDang
    );

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
};