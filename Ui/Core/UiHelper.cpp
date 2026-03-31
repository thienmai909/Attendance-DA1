#include <UiHelper.hpp>

Element UiHelper::makeHeader(const std::string &title, const std::string &sub)
{
    if (sub.empty())
        return text(" " + title + " ") | bold | color(Color::Cyan) | center;
    return vbox({
        text(" " + title + " ") | bold | color(Color::Cyan) | center,
        text(" " + sub + " ") | dim | center
    });
}

Element UiHelper::makeFooter(const std::string &hint)
{
    return text(" " + hint + " ") | dim | center;
}

Element UiHelper::makeTableRow(
    const std::vector<std::pair<std::string, int>> &cols,
    bool highlight
) {
    std::vector<Element> cells;
    for (const auto& [text_, width] : cols)
        cells.push_back(text(text_) | size(WIDTH, EQUAL, width));
    auto row = hbox(cells);
    return highlight ? row | color(Color::Cyan) : row;
}

Element UiHelper::makeMessage(const std::string &msg)
{
    if (msg.empty()) return text("");
    bool ok = msg.starts_with("[OK]");
    return text(" " + msg + " ") | color(ok ? Color::Green : Color::Red) | center;
}
