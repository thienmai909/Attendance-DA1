#include <UiManager.hpp>
#include <LoginScreen.hpp>
#include <MainMenuScreen.hpp>
#include <SinhVienScreen.hpp>
#include <GiangVienScreen.hpp>
#include <LopHocPhanScreen.hpp>
#include <DiemDanhScreen.hpp>

UiManager::UiManager(AppManager &app)
    : _app(app)
{}

void UiManager::run()
{
    auto maGV = screenLogin(_app);
    if (!maGV.has_value()) return;
    _maGVHienTai = *maGV;

    auto giangVienOpt = _app.getGVManager().timTheoMa(_maGVHienTai);
    bool isAdmin      = giangVienOpt.has_value() && giangVienOpt->isAdmin();

    bool thoat = false;
    while (!thoat) {
        int luaChon = screenMainMenu(_app, _maGVHienTai);
        switch(luaChon) {
            case 0: screenDiemDanh(_app, _maGVHienTai); break;
            case 1: screenLopHocPhan(_app, _maGVHienTai); break;
            case 2: if (isAdmin) screenSinhVien(_app); break;
            case 3: if (isAdmin) screenGiangVien(_app); break;
            case 99: thoat = true; break;
        }
    }
}
