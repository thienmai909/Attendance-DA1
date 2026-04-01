#pragma once

#include <AppManager.hpp>
#include <string>

class UiManager {
    AppManager& _app;
    std::string _maGVHienTai;

public:
    explicit UiManager(AppManager& app);
    void run();
};