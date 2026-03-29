// UIManager.hpp
#pragma once

#include <AppManager.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

class UIManager {
    AppManager& _app;

public:
    explicit UIManager(AppManager& app) : _app(app) {}
    void run();
};