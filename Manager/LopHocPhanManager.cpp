#include <LopHocPhanManager.hpp>

LopHocPhanManager::LopHocPhanManager(std::filesystem::path filePath)
    : _filePath(std::move(filePath))
{}

void LopHocPhanManager::load()
{
    _dsLHP = utility_json::load_from_file<LopHocPhan>(
        _filePath, "data", LopHocPhan::fromJson
    );
    _isDirty = false;
}

void LopHocPhanManager::save()
{
    utility_json::save_to_file<LopHocPhan>(
        _filePath, "data", _dsLHP,
        [](const LopHocPhan& lopHocPhan) { return lopHocPhan.toJson(); }
    );
    _isDirty = false;
}

void LopHocPhanManager::saveIfDirty()
{
    if (_isDirty) save();
}

void LopHocPhanManager::them(const LopHocPhan &lopHocPhan)
{
    if (timTheoMa(lopHocPhan.getMaLHP()).has_value())
        throw std::invalid_argument("Mã lớp đã tồn tại: " + lopHocPhan.getMaLHP());
    _dsLHP.push_back(lopHocPhan);
    _isDirty = true;
}

void LopHocPhanManager::capNhat(const LopHocPhan &lopHocPhan)
{
    auto it = timIterator(lopHocPhan.getMaLHP());
    if (it == _dsLHP.end())
        throw std::invalid_argument("Không tim thấy lớp: " + lopHocPhan.getMaLHP());
    *it = lopHocPhan;
    _isDirty = true;
}

void LopHocPhanManager::xoa(const std::string &maLHP)
{
    auto it = timIterator(maLHP);
    if (it == _dsLHP.end())
        throw std::invalid_argument("Không tìm thấy lớp: " + maLHP);
    _dsLHP.erase(it);
    _isDirty = true;
}

std::optional<LopHocPhan> LopHocPhanManager::timTheoMa(const std::string &maLHP) const
{
    for (const auto& lopHocPhan : _dsLHP)
        if (lopHocPhan.getMaLHP() == maLHP) return lopHocPhan;
    return std::nullopt;
}

std::vector<LopHocPhan> LopHocPhanManager::timTheoTen(const std::string &keyword) const
{
    std::vector<LopHocPhan> result;
    for (const auto& lopHocPhan : _dsLHP)
        if (lopHocPhan.matchTen(keyword))
            result.push_back(lopHocPhan);
    return result;
}

std::vector<LopHocPhan> LopHocPhanManager::locTheoHocKi(HocKi hocKi) const
{
    std::vector<LopHocPhan> result;
    for (const auto& lopHocPhan : _dsLHP) 
        if (lopHocPhan.getHocKi() == hocKi) result.push_back(lopHocPhan);
    return result;
}

std::vector<LopHocPhan> LopHocPhanManager::getLopTheoGV(const std::string &maGV) const
{
    std::vector<LopHocPhan> result;
    for (const auto& lopHocPhan : _dsLHP)
        if (lopHocPhan.getMaGV() == maGV)
            result.push_back(lopHocPhan);
    return result;
}

void LopHocPhanManager::themBuoi(
    const std::string &maLHP,
    const DateTime &ngay,
    CaHoc ca,
    int soTiet)
{
    try {
        auto& lopHocPhan = getLopRef(maLHP);
        lopHocPhan.themBuoiDiemDanh(ngay, ca, soTiet);
        _isDirty = true;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

LopHocPhan &LopHocPhanManager::getLopRef(const std::string &maLHP)
{
    auto it = timIterator(maLHP);
    if (it == _dsLHP.end())
        throw std::invalid_argument("Không tìm thấy lớp: " + maLHP);
    return *it;
}

void LopHocPhanManager::markDirty()
{
    _isDirty = true;
}

void LopHocPhanManager::ganGiangVien(const std::string &maLHP, const std::string &maGV)
{
    auto& lopHocPhan = getLopRef(maLHP);
    lopHocPhan.setMaGV(maGV);
    _isDirty = true;
}

void LopHocPhanManager::xoaGiangVien(const std::string &maGV)
{
    auto& lopHocPhan = getLopRef(maGV);
    lopHocPhan.xoaGiangVien();
    _isDirty = true;
}

const std::vector<LopHocPhan> &LopHocPhanManager::getAll() const
{
    return _dsLHP;
}

std::size_t LopHocPhanManager::soLuong() const
{
    return _dsLHP.size();
}

bool LopHocPhanManager::isDirty() const
{
    return _isDirty;
}

std::vector<LopHocPhan>::iterator LopHocPhanManager::timIterator(const std::string &maLHP)
{
    return std::find_if(
        _dsLHP.begin(), _dsLHP.end(),
        [&](const LopHocPhan& lopHocPhan) { return lopHocPhan.getMaLHP() == maLHP; }
    );
}
