#include <AppManager.hpp>
#include <SinhVienManager.hpp>

std::vector<SinhVien>::iterator SinhVienManager::timIterator(const std::string &maSV)
{
    return std::find_if(
        _dsSinhVien.begin(), _dsSinhVien.end(),
        [&](const SinhVien& sinhVien) { return sinhVien.getMaSV() == maSV; }
    );
}

SinhVienManager::SinhVienManager(std::filesystem::path filePath)
    : _filePath(std::move(filePath))
{
}

void SinhVienManager::load()
{
    _dsSinhVien = utility_json::load_from_file<SinhVien>(
        _filePath, "data", SinhVien::fromJson
    );
    _isDirty = false;
}

void SinhVienManager::save()
{
    utility_json::save_to_file<SinhVien>(
        _filePath, "data", _dsSinhVien,
        [](const SinhVien& sinhVien) { return sinhVien.toJson(); }
    );
    _isDirty = false;
}

void SinhVienManager::saveIfDirty()
{
    if (_isDirty) save();
}

void SinhVienManager::them(const SinhVien &sinhVien)
{
    if (!sinhVien.isValid())
        throw std::invalid_argument("Sinh viên không hợp lệ");
    if (timTheoMa(sinhVien.getMaSV()).has_value())
        throw std::invalid_argument("Mã sinh viên đã tồn tại: " + sinhVien.getMaSV());
    _dsSinhVien.push_back(sinhVien);
    _isDirty = true;
}

void SinhVienManager::capNhat(const SinhVien& sinhVien)
{
    auto it = timIterator(sinhVien.getMaSV());
    if (it == _dsSinhVien.end())
        throw std::invalid_argument("Không tìm thấy sinh viên: " + sinhVien.getMaSV());
    *it = sinhVien;
    _isDirty = true;
}

void SinhVienManager::xoa(const std::string &maSV)
{
    auto it = timIterator(maSV);
    if (it == _dsSinhVien.end())
        throw std::invalid_argument("Không tìm thấy sinh viên: " + maSV);
    _dsSinhVien.erase(it);
    _isDirty = true;
}

std::optional<SinhVien> SinhVienManager::timTheoMa(const std::string &maSV) const
{
    for (const auto& sinhVien : _dsSinhVien)
        if (sinhVien.getMaSV() == maSV) 
            return sinhVien;
    return std::nullopt;
}

std::vector<SinhVien> SinhVienManager::timTheoTen(const std::string &keyword) const
{
    std::vector<SinhVien> result;
    for (const auto& sinhVien : _dsSinhVien)
        if (sinhVien.matchTen(keyword))
            result.push_back(sinhVien);
    return result;
}

std::vector<SinhVien> SinhVienManager::locTheoLop(LopSinhHoat lopSinhHoat) const
{
    std::vector<SinhVien> result;
    for (const auto& sinhVien : _dsSinhVien)
        if (sinhVien.getLopSH() == lopSinhHoat)
            result.push_back(sinhVien);
    return result;
}

SinhVien &SinhVienManager::getSinhVienRef(const std::string &maSV)
{
    auto it = timIterator(maSV);
    if (it == _dsSinhVien.end())
        throw std::invalid_argument("Không tìm thấy sinh viên: " + maSV);
    return *it;
}

const std::vector<SinhVien> &SinhVienManager::getAll() const
{
    return _dsSinhVien;
}

std::size_t SinhVienManager::soLuong() const
{
    return _dsSinhVien.size();
}

bool SinhVienManager::isDirty() const
{
    return _isDirty;
}
