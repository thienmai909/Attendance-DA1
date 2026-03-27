#include <GiangVienManager.hpp>

GiangVienManager::GiangVienManager(std::filesystem::path filePath)
    : _filePath(std::move(filePath))
{
}

void GiangVienManager::load()
{
    _dsGiangVien = utility_json::load_from_file<GiangVien>(
        _filePath, "data", GiangVien::fromJson
    );
    _isDirty = false;
}

void GiangVienManager::save()
{
    utility_json::save_to_file<GiangVien>(
        _filePath, "data", _dsGiangVien,
        [] (const GiangVien& giangVien) { return giangVien.toJson(); }
    );
    _isDirty = false;
}

void GiangVienManager::saveIfDirty()
{
    if (_isDirty) save();
}

void GiangVienManager::them(const GiangVien &giangVien)
{
    if (!giangVien.isValid())
        throw std::invalid_argument("Giảng viên không hợp lệ!");
    if (timTheoMa(giangVien.getMaGV()).has_value())
        throw std::invalid_argument("Mã GV đã tồn tại: " + giangVien.getMaGV());
    _dsGiangVien.push_back(giangVien);
    _isDirty = true;
}

void GiangVienManager::capNhat(const GiangVien &giangVien)
{
    auto it = timIterator(giangVien.getMaGV());
    if (it == _dsGiangVien.end())
        throw std::invalid_argument("Không tìm thấy GV: " + giangVien.getMaGV());
    *it = giangVien;
    _isDirty = true;
}

void GiangVienManager::xoa(const std::string &maGV)
{
    auto it = timIterator(maGV);
    if (it == _dsGiangVien.end())
        throw std::invalid_argument("Không tìm thấy GV: " + maGV);
    _dsGiangVien.erase(it);
    _isDirty = true;
}

std::vector<GiangVien>::iterator GiangVienManager::timIterator(const std::string &maGV){
    return std::find_if(
        _dsGiangVien.begin(), _dsGiangVien.end(),
        [&](const GiangVien &giangVien)
        { return giangVien.getMaGV() == maGV; }
    ); 
}

std::optional<GiangVien> GiangVienManager::timTheoMa(const std::string &maGV) const
{
    for (const auto& giangVien : _dsGiangVien) 
        if (giangVien.getMaGV() == maGV) return giangVien;
    return std::nullopt;
}

std::optional<GiangVien> GiangVienManager::timTheoTaiKhoan(const std::string &username) const
{
    for (const auto& giangVien : _dsGiangVien) 
        if (giangVien.getTenTaiKhoan() == username) return giangVien;
    return std::nullopt;
}

std::vector<GiangVien> GiangVienManager::timTheoTen(const std::string &keyword) const
{
    std::vector<GiangVien> result;
    for (const auto& giangVien : _dsGiangVien)
        if (giangVien.matchHoTen(keyword)) result.push_back(giangVien);
    return result;
}

std::optional<GiangVien> GiangVienManager::dangNhap(
    const std::string &username,
    const std::string &password
) const
{
    auto giangVien = timTheoTaiKhoan(username);
    if (giangVien.has_value())
        if (giangVien->xacThucTaiKhoan(username, password)) return giangVien;
    return std::nullopt;
}

const std::vector<GiangVien> &GiangVienManager::getAll() const
{
    return _dsGiangVien;
}

std::size_t GiangVienManager::soLuong() const
{
    return _dsGiangVien.size();

}

bool GiangVienManager::isDirty() const
{
    return _isDirty;
}
