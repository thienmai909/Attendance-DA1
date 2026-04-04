#pragma once

#include <AppManager.hpp>
#include <string>

bool formThemSinhVien(AppManager& app);
bool formSuaSinhVien(AppManager& app, const std::string& maSV);
bool formXoaSinhVien(AppManager& app, const std::string& maSV);