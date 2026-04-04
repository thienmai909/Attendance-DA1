#include <MainMenuScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

int screenMainMenu(AppManager &app, const std::string &maGV)
{
    auto screen = ScreenInteractive::Fullscreen();

    std::string tenGV = "Giảng viên";
    auto giangVien = app.getGVManager().timTheoMa(maGV);
    if (giangVien.has_value())
        tenGV = giangVien->getHoTenGV();

    auto entries = std::vector<std::string> {
        "[D] Điểm danh hôm nay",
        "[L] Quản lý lớp học phần",
        "[S] Quản lý sinh viên",
        "[B] Báo cáo & Thống kê",
        "[Q] Thoát"
    };

    int selected = 0;
    int ketQua = 4;
    auto menu = Menu(&entries, &selected, MenuOption::Vertical());

    auto renderer = Renderer(menu, [&] {
        int soLop = static_cast<int>(app.getLHPManager().getAll().size());
        int soSV = static_cast<int>(app.getSVManager().getAll().size());

        return vbox({
            UiHelper::makeHeader("HỆ THỐNG ĐIỂM DANH"),
            separator(),
            hbox({
                vbox({
                    text(" MENU CHÍNH ") | bold | center,
                    separator(),
                    menu->Render() | flex
                }) | border | size(WIDTH, EQUAL, 36),
                vbox({
                    text(" THÔNG TIN ") | bold | center,
                    separator(),
                    hbox({ text(" Xin chào: ") | dim, text(tenGV) | bold }),
                    separator(),
                    hbox({ text(" Số lớp : ") | dim, text(std::to_string(soLop)) | bold }),
                    hbox({ text(" Số SV  : ") | dim, text(std::to_string(soSV)) | bold }),
                    filler()
                }) | border | flex
            }),
            separator(),
            UiHelper::makeFooter("[↑↓] Di chuyển  [Enter] Chọn  [Q] Thoát")
        });
    }) | CatchEvent([&] (Event e) {
        if (e == Event::Character('d') || e == Event::Character('D'))
            { ketQua = 0; screen.Exit(); return true; }
        if (e == Event::Character('l') || e == Event::Character('L'))
            { ketQua = 1; screen.Exit(); return true; }
        if (e == Event::Character('s') || e == Event::Character('S'))
            { ketQua = 2; screen.Exit(); return true; }
        if (e == Event::Character('b') || e == Event::Character('B'))
            { ketQua = 3; screen.Exit(); return true; }
        if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape)
            { ketQua = 4; screen.Exit(); return true; }
        if (e == Event::Return)
            { ketQua = selected; screen.Exit(); return true; }
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}