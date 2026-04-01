#include <UiManager.hpp>
#include <LoginScreen.hpp>
#include <MainMenuScreen.hpp>

UiManager::UiManager(AppManager &app)
    : _app(app)
{}

void UiManager::run()
{
    auto maGV = screenLogin(_app);
    if (!maGV.has_value()) return;
    _maGVHienTai = *maGV;

    bool thoat = false;
    while (!thoat) {
        int luaChon = screenMainMenu(_app, _maGVHienTai);
        switch(luaChon) {
            case 4: thoat = true; break;
        }
    }
}
