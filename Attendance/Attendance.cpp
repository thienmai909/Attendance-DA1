#include <iostream>
#include <filesystem>

#include <AppManager.hpp>
#include <UiManager.hpp>

int main() {
    AppManager app;
    std::filesystem::create_directories("data");
    app.khoiDong();
    UIManager ui(app);
    ui.run();
    app.dongLai();
    return 0;
}