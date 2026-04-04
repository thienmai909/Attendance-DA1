#include <SinhVienScreen.hpp>
#include <SinhVienForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

void screenSinhVien(AppManager &app)
{
    bool thoat = false;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();

        const auto& danhSach = app.getSVManager().getAll();
        int selected = 0;
        int luaChon = -1;

        std::vector<std::string> entries;
        for (const auto& sinhVien : danhSach)
            entries.push_back(sinhVien.getMaSV() + " " + sinhVien.getTenSV());
        if (entries.empty())
            entries.push_back("(Chưa có sinh viên)");
        
        auto menuSinhVien = Menu(&entries, &selected);

        auto menuThaoTac = std::vector<std::string>{
            "[T] Thêm sinh viên",
            "[S] Sửa sinh viên",
            "[X] Xóa sinh viên",
            "[Q] Quay lại",
        };
        int selectedThaoTac = 0;
        auto menuAction = Menu(&menuThaoTac, &selectedThaoTac);

        auto layout = Container::Horizontal({ menuSinhVien, menuAction });
        auto renderer = Renderer(layout, [&] {
            Element chiTiet = filler();
            if (!danhSach.empty() && selected < static_cast<int>(danhSach.size())) {
                const auto& sinhVien = danhSach[selected];
                chiTiet = vbox({
                    text(" CHI TIẾT ") | bold | center,
                    separator(),
                    hbox({ text(" Mã SV     : ") | dim, text(sinhVien.getMaSV()) | bold }),
                    hbox({ text(" Họ tên    : ") | dim, text(sinhVien.getTenSV()) | bold }),
                    hbox({ text(" Lớp SH    : ") | dim, text(sinhVien.getLopSHStr()) }),
                    hbox({ text(" Ngày sinh : ") | dim, text(sinhVien.getNgaySinhStr()) }),
                    hbox({ text(" Liên hệ   : ") | dim, text(sinhVien.getLienHeStr()) }),
                    filler()
                });
            }

            return vbox({
                UiHelper::makeHeader("QUẢN LÝ SINH VIÊN"),
                separator(),
                hbox({
                    vbox({
                        text(" THAO TÁC ") | bold | center,
                        separator(),
                        menuAction->Render(),
                        filler()
                    }) | border | size(WIDTH, EQUAL, 18),
                    vbox({
                        hbox({
                            text(" DANH SÁCH (") | dim,
                            text(std::to_string(danhSach.size())) | bold,
                            text(") ") | dim
                        }) | center,
                        separator(),
                        menuSinhVien->Render() | flex
                    })| border | flex,

                    chiTiet | border | size(WIDTH, EQUAL, 50)                    
                }) | flex,
                separator(),
                UiHelper::makeFooter("[↑↓] Chọn  [T]hêm  [S]ửa  [X]óa  [Q]uay lại")
            });
        }) | CatchEvent([&] (Event e) {
            if (e == Event::Return && layout->ActiveChild() == menuAction) {
                luaChon = selectedThaoTac;
                screen.Exit();
                return true;
            }

            if (e == Event::Character('t') || e == Event::Character('T'))
                { luaChon = 0; screen.Exit(); return true; }
            if (e == Event::Character('s') || e == Event::Character('S'))
                { luaChon = 1; screen.Exit(); return true; }
            if (e == Event::Character('x') || e == Event::Character('X'))
                { luaChon = 2; screen.Exit(); return true; }
            if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape)
                { luaChon = 3; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);

        if (!danhSach.empty() && selected < static_cast<int>(danhSach.size())) {
            std::string maSVChon = danhSach[selected].getMaSV();
            switch (luaChon) {
                case 0: formThemSinhVien(app); break;
                case 1: break;
                case 2: break;
                case 3: thoat = true; break;
            }
        } else {
            if (luaChon == 0) formThemSinhVien(app);
            else thoat = true;
        }
    }
}