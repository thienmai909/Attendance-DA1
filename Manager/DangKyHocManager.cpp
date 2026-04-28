#include <DangKyHocManager.hpp>

DangKyHocManager::DangKyHocManager(std::filesystem::path filePath)
    : _filePath(std::move(filePath))
{
}

void DangKyHocManager::load()
{
    _dsDangKy = utility_json::load_from_file<DangKyHoc>(
        _filePath, "data", DangKyHoc::fromJson
    );
    _rebuildIndex();
    _isDirty = false;
}

void DangKyHocManager::save()
{
    utility_json::save_to_file<DangKyHoc>(
        _filePath, "data", _dsDangKy,
        [](const DangKyHoc& dangKyHoc) { return dangKyHoc.toJson(); }
    );
    _isDirty = false;
}

void DangKyHocManager::saveIfDirty()
{
    if (_isDirty) save();
}

void DangKyHocManager::dangKy(const std::string &maSV, const std::string &maLHP)
{
    auto it = std::find_if(
        _dsDangKy.begin(), _dsDangKy.end(),
        [&](const DangKyHoc& dk) { return dk.trungVoi(maSV, maLHP); }
    );

    if (it != _dsDangKy.end()) {
        if (it->isActive())
            throw std::invalid_argument(
                "SV " + maSV + " da dang ky lop " + maLHP + "!");
        else
            it->kichHoatLai();
    } else {
        _dsDangKy.emplace_back(maSV, maLHP, DateTime(), true);
    }
    _rebuildIndex();
    _isDirty = true;
}

void DangKyHocManager::huyDangKy(const std::string &maSV, const std::string &maLHP)
{
    auto it = timIterator(maSV, maLHP);
    if (it == _dsDangKy.end())
        throw std::invalid_argument("Không tìm thấy đăng ký!");
    it->huyDangKy();
    _rebuildIndex();
    _isDirty = true;
}

void DangKyHocManager::kichHoatLai(const std::string &maSV, const std::string &maLHP)
{
    auto it = timIterator(maSV, maLHP);
    if (it == _dsDangKy.end())
        throw std::invalid_argument("Không tìm thấy đăng ký!");
    it->kichHoatLai();
    _rebuildIndex();
    _isDirty = true;
}

bool DangKyHocManager::daDangKy(const std::string &maSV, const std::string &maLHP) const
{
    for (const auto& dangKy : _dsDangKy)
        if (dangKy.trungVoi(maSV, maLHP) && dangKy.isActive()) return true;
    return false;
}

const std::vector<std::string>& DangKyHocManager::getDsMaSVTheoLop(const std::string &maLHP) const
{
    static const std::vector<std::string> empty;
    auto it = _indexByLHP.find(maLHP);
    return (it != _indexByLHP.end()) ? it->second : empty;
}

std::vector<std::string> DangKyHocManager::getDsMaLHPTheoSV(const std::string &maSV) const
{
    std::vector<std::string> result;
    for (const auto& dangKy : _dsDangKy)
        if (dangKy.getMaSV() == maSV && dangKy.isActive())
            result.push_back(dangKy.getMaLopHocPhan()); // FIX #1: push maLHP, khong phai maSV
    return result;
}

std::vector<DangKyHoc> DangKyHocManager::getLichSuTheoSV(const std::string &maSV) const
{
    std::vector<DangKyHoc> result;
    for (const auto& dangKy : _dsDangKy)
        if (dangKy.getMaSV() == maSV) result.push_back(dangKy);
    return result;
}

std::vector<DangKyHoc> DangKyHocManager::getLichSuTheoLop(const std::string &maLHP) const
{
    std::vector<DangKyHoc> result;
    for (const auto& dangKy : _dsDangKy)
        if (dangKy.getMaLopHocPhan() == maLHP) result.push_back(dangKy);
    return result;
}

const std::vector<DangKyHoc> &DangKyHocManager::getAll() const
{
    return _dsDangKy;
}

std::size_t DangKyHocManager::soLuong() const
{
    return _dsDangKy.size();
}

bool DangKyHocManager::isDirty() const
{
    return _isDirty;
}

std::vector<DangKyHoc>::iterator DangKyHocManager::timIterator(
    const std::string &maSV,
    const std::string &maLHP)
{
    return std::find_if(
        _dsDangKy.begin(),
        _dsDangKy.end(),
        [&](const DangKyHoc& dangKyHoc) { return dangKyHoc.trungVoi(maSV, maLHP); }
    );
}

void DangKyHocManager::_rebuildIndex()
{
    _indexByLHP.clear();
    for (const auto& dk : _dsDangKy)
        if (dk.isActive())
            _indexByLHP[dk.getMaLopHocPhan()].push_back(dk.getMaSV());
}
