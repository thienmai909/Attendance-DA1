#include <MainMenuScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

int screenMainMenu(AppManager &app, const std::string &maGV)
{
    auto screen = ScreenInteractive::Fullscreen();
    int ketQua = -1;

    auto giangVienOpt = app.getGVManager().timTheoMa(maGV);
    bool isAdmin = giangVienOpt.has_value() && giangVienOpt->isAdmin();
    std::string tenGV = giangVienOpt.has_value() ? giangVienOpt->getHoTenGV() : maGV;

    std::vector<std::string> entries;
    std::vector<int>         indices;

    // if (!isAdmin)
    entries.push_back("  [D]  Điểm danh");        indices.push_back(0);
    entries.push_back("  [L]  Quản lý lớp HP");       indices.push_back(1);

    if (isAdmin) {
        entries.push_back("  [S]  Quản lý sinh viên");  indices.push_back(2);
        entries.push_back("  [G]  Quản lý giảng viên"); indices.push_back(3);
    }

    entries.push_back("  [B]  Báo cáo & thống kê"); indices.push_back(4);
    entries.push_back("  [Q]  Thoát"); indices.push_back(99);

    int selected = 0;
    auto menu = Menu(&entries, &selected, MenuOption::Vertical());

    auto renderer = Renderer(menu, [&] {
        int soLop = static_cast<int>(app.getLHPManager().getAll().size());
        int soSV  = static_cast<int>(app.getSVManager().getAll().size());
        int soGV  = static_cast<int>(app.getGVManager().getAll().size());

        return vbox({
            UiHelper::makeHeader("HỆ THỐNG ĐIỂM DANH"),
            separator(),
            hbox({
                vbox({
                    text(" MENU ") | bold | center,
                    separator(),
                    menu->Render() | flex
                }) | border | size(WIDTH, EQUAL, 36),

                vbox({
                    text(" THÔNG TIN ") | bold | center,
                    separator(),
                    hbox({ text(" Xin chào  : ") | dim, text(tenGV) | bold }),
                    hbox({ text(" Quyền     : ") | dim,
                        isAdmin
                            ? text("Admin") | color(Color::Yellow) | bold
                            : text("Giảng viên") | dim
                    }),
                    separator(),
                    hbox({ text(" Số lớp    : ") | dim, text(std::to_string(soLop)) | bold }),
                    hbox({ text(" Số SV     : ") | dim, text(std::to_string(soSV))  | bold }),
                    hbox({ text(" Số GV     : ") | dim, text(std::to_string(soGV))  | bold }),
                    filler()
                }) | border | flex
            }) | flex,
            separator(),
            UiHelper::makeFooter(
                isAdmin
                    ? "[D/L/S/G/B/Q] Phím tắt  [↑↓] Di chuyển  [Enter] Chọn"
                    : "[D/L/B/Q] Phím tắt  [↑↓] Di chuyển  [Enter] Chọn"
            )
        });
    }) | CatchEvent([&](Event e) {
        if (e == Event::Return)
            { ketQua = indices[selected]; screen.Exit(); return true; }

        // if (!isAdmin)
        if (e == Event::Character('d') || e == Event::Character('D'))
            { ketQua = 0;  screen.Exit(); return true; }

        if (e == Event::Character('l') || e == Event::Character('L'))
            { ketQua = 1;  screen.Exit(); return true; }
            
        if (isAdmin) {
            if (e == Event::Character('s') || e == Event::Character('S'))
                { ketQua = 2;  screen.Exit(); return true; }
            if (e == Event::Character('g') || e == Event::Character('G'))
                { ketQua = 3;  screen.Exit(); return true; }
        }

        if (e == Event::Character('b') || e == Event::Character('B'))
            { ketQua = 4;  screen.Exit(); return true; }
        if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape)
            { ketQua = 99; screen.Exit(); return true; }
            
        return false;
    });

    screen.Loop(renderer);
    return ketQua;
}