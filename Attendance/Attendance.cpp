#include <iostream>
#include <filesystem>

#include <AppManager.hpp>
#include <UiManager.hpp>
#include <Utility.hpp>

int main() {
    utility_log::Logger::instance().init("logs/app.log");

    std::filesystem::create_directories("data");

    AppManager app;
    try {
        app.khoiDong();
    } catch (const std::exception& e) {
        LOG_ERROR("Main", "Khoi dong that bai: " + std::string(e.what()));
        return 1;
    }

    UiManager ui(app);
    ui.run();

    try {
        app.dongLai();
        LOG_INFO("Main", "Luu data thanh cong");
    } catch (const std::exception& e) {
        LOG_ERROR("Main", "Luu data that bai: " + std::string(e.what()));
    }

    return 0;
}