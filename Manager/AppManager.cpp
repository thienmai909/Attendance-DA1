#include <AppManager.hpp>

AppManager::AppManager()
    : _svManager("data/sinhvien.json")
    , _gvManager("data/giangvien.json")
    , _lhpManager("data/lophocphan.json")
    , _dkhManager("data/dangkyhoc.json")
    , _ddManager(_lhpManager)
{}

void AppManager::khoiDong()
{
    _svManager.load();
    _gvManager.load();
    _dkhManager.load();
    _lhpManager.load();
}

void AppManager::dongLai()
{
    _svManager.saveIfDirty();
    _gvManager.saveIfDirty();
    _dkhManager.saveIfDirty();
    _lhpManager.saveDirty();
}

SinhVienManager &AppManager::getSVManager()
{
    return _svManager;
}

GiangVienManager &AppManager::getGVManager()
{
    return _gvManager;
}

DangKyHocManager &AppManager::getDKHManager()
{
    return _dkhManager;
}

LopHocPhanManager &AppManager::getLHPManager()
{
    return _lhpManager;
}
