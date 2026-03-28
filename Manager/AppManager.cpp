#include <AppManager.hpp>

AppManager::AppManager()
    : _svManager("data/sinhvien.json")
    , _gvManager("data/giangvien.json")
    , _lhpManager("data/lophocphan.json")
    , _dkManager("data/dangkyhoc.json")
    , _ddManager(_lhpManager)
    , _tkManager(_lhpManager, _svManager, _dkManager)
{}

void AppManager::khoiDong()
{
    _svManager.load();
    _gvManager.load();
    _dkManager.load();
    _lhpManager.load();
}

void AppManager::dongLai()
{
    _svManager.saveIfDirty();
    _gvManager.saveIfDirty();
    _dkManager.saveIfDirty();
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

DangKyHocManager &AppManager::getDKManager()
{
    return _dkManager;
}

LopHocPhanManager &AppManager::getLHPManager()
{
    return _lhpManager;
}

ThongKeManager &AppManager::getTKManager()
{
    return _tkManager;
}
