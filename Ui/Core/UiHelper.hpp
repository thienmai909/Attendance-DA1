#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

using namespace ftxui;

namespace UiHelper {
    inline Element makeHeader(
        const std::string& title,
        const std::string& sub = ""
    ) {
        if (sub.empty())
            return text(" " + title + " ") | bold | color(Color::Black) | center;
        return vbox({
                text(" " + title + " ") | bold | color(Color::Black) | center,
                text(" " + sub   + " ") | dim  | center
        });
    }

    inline Element makeFooter(const std::string& hint) {
        return text(" " + hint + " ") | dim | center;
    }

    inline Element makeMessage(const std::string& msg) {
        if (msg.empty())
            return text(" ");

        bool ok = msg.starts_with("[OK]");

        return text(" " + msg + " ")
            | color(ok ? Color::Green : Color::Red)
            | center;
    }

    inline Element makeRow(
        const std::vector<std::pair<std::string, int>>& cols,
        bool selected = false
    ) {
        std::vector<Element> cells;
        for (const auto& [txt, w] : cols)
            cells.push_back(text(txt) | size(WIDTH, EQUAL, w));
        auto row = hbox(cells);
        return selected ? (row | color(Color::Cyan) | bold) : row;
    }

    inline Element makeTableHeader(
        const std::vector<std::pair<std::string, int>>& cols
    ) {
        std::vector<Element> cells;
        for (const auto& [txt, w] : cols)
            cells.push_back(text(txt) | bold | size(WIDTH, EQUAL, w));
        return hbox(cells) | color(Color::Yellow);
    }

    inline Element makeSeparatorLabel(const std::string& label) {
        return hbox({
            separator(),
            text(" " + label + " ") | dim,
            separator()
        });    
    }

    inline Element makeConfirm(const std::string& question) {
        return vbox({
            text(" " + question + " ") | center,
            separator(),
            hbox({
                text(" [Enter] Xac nhan ") | color(Color::Green),
                text("  "),
                text(" [Q] Huy ") | color(Color::Red)
            }) | center
        }) | border | color(Color::Yellow);
    }

}