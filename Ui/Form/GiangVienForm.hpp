#pragma once

#include <AppManager.hpp>
#include <string>

bool formThemGiangVien(AppManager& app);
bool formSuaGiangVien(AppManager& app, const std::string& maGV);
bool formXoaGiangVien(AppManager& app, const std::string& maGV);