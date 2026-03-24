#include <AppManager.hpp>

void AppManager::khoiDong()
{
    _svManager.load();
}

void AppManager::dongLai()
{
    _svManager.saveIfDirty();
}

SinhVienManager &AppManager::getSVManager()
{
    return _svManager;
}
