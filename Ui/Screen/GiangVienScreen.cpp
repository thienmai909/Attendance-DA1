#include <GiangVienScreen.hpp>
#include <GiangVienForm.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

void screenGiangVien(AppManager &app)
{
    bool thoat    = false;
    int  selected = 0;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();

        const auto& danhSachGV = app.getGVManager().getAll();
        int luaChon = -1;

        if (!danhSachGV.empty())
            selected = std::min(selected, static_cast<int>(danhSachGV.size()) - 1);
        else
            selected = 0;

        std::vector<std::string> entries;
        for (const auto& giangVien : danhSachGV) {
            std::string line = giangVien.getMaGV() + "  " + giangVien.getHoTenGV();
            if (giangVien.isAdmin()) line += " [Admin]";
            entries.push_back(line);
        }

        if (entries.empty())
            entries.push_back("(Chưa có giảng viên)");
        
        auto menuGV = Menu(&entries, &selected);

        auto thaoTacEntries = std::vector<std::string>{
            "[T] Thêm GV",
            "[S] Sửa GV",
            "[X] Xóa GV",
            "[Q] Quay lại"
        };

        int  selectedAction  = 0;
        auto menuAction = Menu(&thaoTacEntries, &selectedAction);
        auto layout     = Container::Horizontal({ menuGV, menuAction });

        auto renderer = Renderer(layout, [&] {
            Element chiTiet = filler();
            if (!danhSachGV.empty() && selected < static_cast<int>(danhSachGV.size())) {
                const auto& giangVien = danhSachGV[selected];
                int soLop = static_cast<int>(
                    app.getLHPManager().getLopTheoGV(giangVien.getMaGV()).size()
                );
                chiTiet = vbox({
                    text(" CHI TIẾT ") | bold | center,
                    separator(),
                    hbox({ text(" Mã GV      : ") | dim, text(giangVien.getMaGV())       | bold }),
                    hbox({ text(" Họ tên     : ") | dim, text(giangVien.getHoTenGV())    | bold }),
                    hbox({ text(" Học vị     : ") | dim, text(giangVien.getHocViStr())   }),
                    hbox({ text(" Tài khoản  : ") | dim, text(giangVien.getTenTaiKhoan()) }),
                    hbox({ text(" Liên hệ    : ") | dim, text(giangVien.getLienHeStr())  }),
                    hbox({ text(" Số lớp     : ") | dim, text(std::to_string(soLop)) | bold }),
                    hbox({ text(" Quyền      : ") | dim,
                        giangVien.isAdmin()
                            ? text("Admin") | color(Color::Yellow) | bold
                            : text("Giảng viên") | dim
                    }),
                    filler()
                });
            }

            return vbox({
                UiHelper::makeHeader("QUẢN LÝ GIẢNG VIÊN"),
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
                            text(std::to_string(danhSachGV.size())) | bold,
                            text(") ") | dim
                        }) | center,
                        separator(),
                        menuGV->Render() | flex
                    }) | border | flex,

                    chiTiet | border | size(WIDTH, EQUAL, 34)
                }) | flex,
                separator(),
                UiHelper::makeFooter("[↑↓] Chọn  [T]hêm  [S]ửa  [X]óa  [Q]uay lại")
            });
        }) | CatchEvent([&](Event e) {
            if (e == Event::Return && layout->ActiveChild() == menuAction)
                { luaChon = selectedAction; screen.Exit(); return true; }
            if (e == Event::Character('t') || e == Event::Character('T'))
                { luaChon = 0; screen.Exit(); return true; }
            if (e == Event::Character('s') || e == Event::Character('S'))
                { luaChon = 1; screen.Exit(); return true; }
            if (e == Event::Character('x') || e == Event::Character('X'))
                { luaChon = 2; screen.Exit(); return true; }
            if (e == Event::Character('q') || e == Event::Character('Q')
             || e == Event::Escape)
                { luaChon = 3; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);

        std::string maGVChon = (!danhSachGV.empty() && selected < static_cast<int>(danhSachGV.size()))
            ? danhSachGV[selected].getMaGV() : "";

        switch (luaChon) {
            case 0: formThemGiangVien(app);                          break;
            case 1: if (!maGVChon.empty()) formSuaGiangVien(app, maGVChon); break;
            case 2:
                if (!maGVChon.empty()) {
                    auto giangVien = app.getGVManager().timTheoMa(maGVChon);
                    if (giangVien.has_value() && giangVien->isAdmin())
                        LOG_WARNING("GiangVienScreen", "Không thể xóa tài khoản Admin");
                    else
                        formXoaGiangVien(app, maGVChon);
                }
                break;
            case 3: thoat = true; break;
        }
    }
}