#include <PhongHocManager.hpp>
#include <Utility.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <algorithm>

// ---------- JSON helpers (ClassRoom khong co toJson rieng) ----------
static nlohmann::json phongToJson(const ClassRoom& p) {
    return {
        {"maPhong",  p.getTenPhong()},
        {"sucChua",  p.getSucChua()},
        {"loai",     p.getLoaiPhong() == RoomType::PhongThucHanh ? 1 : 0}
    };
}

static ClassRoom phongFromJson(const nlohmann::json& j) {
    std::string ma  = j.value("maPhong", "");
    int sc          = j.value("sucChua", 0);
    int loai        = j.value("loai", 0);
    return ClassRoom(ma, sc, loai == 1 ? RoomType::PhongThucHanh
                                       : RoomType::PhongLyThuyet);
}

// ---------- Constructor ----------
PhongHocManager::PhongHocManager(std::filesystem::path filePath)
    : _filePath(std::move(filePath)) {}

// ---------- Load / Save ----------
void PhongHocManager::load() {
    _dsPhong.clear();
    if (!std::filesystem::exists(_filePath)) { _isDirty = false; return; }
    try {
        std::ifstream f(_filePath);
        auto root = nlohmann::json::parse(f);
        if (root.contains("data") && root["data"].is_array()) {
            for (const auto& item : root["data"])
                _dsPhong.push_back(phongFromJson(item));
        }
    } catch (...) {}
    _isDirty = false;
}

void PhongHocManager::save() {
    std::filesystem::create_directories(_filePath.parent_path());
    nlohmann::json root;
    root["data"] = nlohmann::json::array();
    for (const auto& p : _dsPhong)
        root["data"].push_back(phongToJson(p));
    std::ofstream f(_filePath);
    f << root.dump(2);
    _isDirty = false;
}

void PhongHocManager::saveIfDirty() {
    if (_isDirty) save();
}

// ---------- CRUD ----------
void PhongHocManager::them(const std::string& maPhong, int sucChua, RoomType loai) {
    if (maPhong.empty())
        throw std::invalid_argument("Mã phòng không được để trống!");
    if (timTheoMa(maPhong).has_value())
        throw std::invalid_argument("Phòng đã tồn tại: " + maPhong);
    _dsPhong.emplace_back(maPhong, sucChua, loai);
    _isDirty = true;
}

void PhongHocManager::xoa(const std::string& maPhong) {
    auto it = std::find_if(_dsPhong.begin(), _dsPhong.end(),
        [&](const ClassRoom& p){ return p.getTenPhong() == maPhong; });
    if (it == _dsPhong.end())
        throw std::invalid_argument("Không tìm thấy phòng: " + maPhong);
    _dsPhong.erase(it);
    _isDirty = true;
}

std::optional<ClassRoom> PhongHocManager::timTheoMa(const std::string& maPhong) const {
    for (const auto& p : _dsPhong)
        if (p.getTenPhong() == maPhong) return p;
    return std::nullopt;
}

const std::vector<ClassRoom>& PhongHocManager::getAll() const { return _dsPhong; }
std::size_t PhongHocManager::soLuong() const { return _dsPhong.size(); }
bool PhongHocManager::isDirty() const { return _isDirty; }
