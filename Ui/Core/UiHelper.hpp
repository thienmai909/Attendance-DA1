#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

using namespace ftxui;

namespace UiHelper {
    inline Element makeHeader(const std::string& title, const std::string& sub = "");
    inline Element makeFooter(const std::string& hint);
    inline Element makeTableRow(
        const std::vector<std::pair<std::string, int>>& cols,
        bool highlight = false
    );
    inline Element makeMessage(const std::string& msg);
}