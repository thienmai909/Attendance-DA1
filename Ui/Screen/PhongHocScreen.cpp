#include <PhongHocScreen.hpp>
#include <UiHelper.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

void screenPhongHoc(AppManager& app) {
    bool thoat   = false;
    int selected = 0;
    std::string thongBao;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();
        int luaChon = -1;

        // Du lieu
        const auto& dsPhong = app.getPHManager().getAll();
        if (!dsPhong.empty())
            selected = std::min(selected, static_cast<int>(dsPhong.size()) - 1);
        else
            selected = 0;

        std::vector<std::string> entries;
        for (const auto& p : dsPhong)
            entries.push_back(p.getTenPhong() + "  [" +
                              p.getLoaiPhongStr() + "]  " +
                              std::to_string(p.getSucChua()) + " chỗ");
        if (entries.empty())
            entries.push_back("(Chưa có phòng nào)");

        // Input them phong
        std::string inputMa, inputSCStr;
        int selectedLoai = 0;
        std::vector<std::string> loaiEntries = {"Lý thuyết", "Thực hành"};
        InputOption opt; opt.multiline = false;
        opt.transform = [](InputState state) -> Element { return state.element | color(Color::Black); };
        auto inputMaComp  = Input(&inputMa,    "B2-01", opt);
        auto inputSCComp  = Input(&inputSCStr, "50",    opt);
        auto menuLoai     = Radiobox(&loaiEntries, &selectedLoai);

        auto menuPhong    = Menu(&entries, &selected);

        auto btnThem = Button("Thêm [T]", [&] {
            if (inputMa.empty()) {
                thongBao = "[ERR] Mã phòng không được trống!";
                return;
            }
            int sc = 0;
            try { if (!inputSCStr.empty()) sc = std::stoi(inputSCStr); }
            catch (...) { thongBao = "[ERR] Sức chứa không hợp lê!"; return; }
            RoomType loai = (selectedLoai == 1)
                ? RoomType::PhongThucHanh : RoomType::PhongLyThuyet;
            try {
                app.getPHManager().them(inputMa, sc, loai);
                thongBao = "[OK] Đã thêm phòng: " + inputMa;
                inputMa.clear(); inputSCStr.clear();
                luaChon = 0; screen.Exit();
            } catch (const std::exception& e) {
                thongBao = std::string("[ERR] ") + e.what();
            }
        });

        auto btnXoa = Button("Xóa [X]", [&] {
            if (dsPhong.empty() || selected >= static_cast<int>(dsPhong.size())) {
                thongBao = "[ERR] Chưa chọn phòng!";
                return;
            }
            try {
                std::string ma = dsPhong[selected].getTenPhong();
                app.getPHManager().xoa(ma);
                thongBao = "[OK] Đã xóa phòng: " + ma;
                selected = std::max(0, selected - 1);
                luaChon = 0; screen.Exit();
            } catch (const std::exception& e) {
                thongBao = std::string("[ERR] ") + e.what();
            }
        });

        auto btnQuayLai = Button("Quay lại [Q]", [&] {
            luaChon = 99; screen.Exit();
        });

        auto layout = Container::Vertical({
            inputMaComp, inputSCComp, menuLoai,
            Container::Horizontal({ btnThem, btnXoa, btnQuayLai }),
            menuPhong
        });

        auto renderer = Renderer(layout, [&] {
            Element chiTiet = filler();
            if (!dsPhong.empty() && selected < static_cast<int>(dsPhong.size())) {
                const auto& p = dsPhong[selected];
                chiTiet = vbox({
                    text(" CHI TIẾT ") | bold | center,
                    separator(),
                    hbox({ text(" Mã phòng  : ") | dim, text(p.getTenPhong()) | bold }),
                    hbox({ text(" Loại      : ") | dim, text(p.getLoaiPhongStr()) }),
                    hbox({ text(" Sức chứa  : ") | dim,
                           text(std::to_string(p.getSucChua()) + " chỗ") | bold }),
                    filler()
                });
            }

            return vbox({
                UiHelper::makeHeader("QUẢN LÝ PHÒNG HỌC", "Admin"),
                separator(),
                hbox({
                    // Vung them phong
                    vbox({
                        text(" THÊM PHÒNG ") | bold | center,
                        separator(),
                        hbox({ text(" Mã phòng : ") | size(WIDTH, EQUAL, 12),
                               inputMaComp->Render() | size(WIDTH, EQUAL, 16) }),
                        hbox({ text(" Sức chứa : ") | size(WIDTH, EQUAL, 12),
                               inputSCComp->Render() | size(WIDTH, EQUAL, 8) }),
                        hbox({ text(" Loại     : ") | size(WIDTH, EQUAL, 12),
                               menuLoai->Render() }),
                        separator(),
                        hbox({ btnThem->Render(), text(" "),
                               btnXoa->Render(),  text(" "),
                               btnQuayLai->Render() }) | center,
                    }) | border | size(WIDTH, EQUAL, 40),

                    // Danh sach phong
                    vbox({
                        hbox({
                            text(" DANH SÁCH (") | dim,
                            text(std::to_string(dsPhong.size())) | bold,
                            text(") ") | dim
                        }) | center,
                        separator(),
                        menuPhong->Render() | flex
                    }) | border | flex,

                    // Chi tiet
                    chiTiet | border | size(WIDTH, EQUAL, 36)
                }) | flex,
                separator(),
                UiHelper::makeMessage(thongBao),
                UiHelper::makeFooter("[T] Thêm  [X] Xóa  [Q] Quay lại  [Tab] Chuyển ô")
            });
        }) | CatchEvent([&](Event e) {
            if (e == Event::Character('t') || e == Event::Character('T'))
                { btnThem->OnEvent(Event::Return); return true; }
            if (e == Event::Character('x') || e == Event::Character('X'))
                { btnXoa->OnEvent(Event::Return); return true; }
            if (e == Event::Character('q') || e == Event::Character('Q') || e == Event::Escape)
                { luaChon = 99; screen.Exit(); return true; }
            return false;
        });

        screen.Loop(renderer);
        if (luaChon == 99) thoat = true;
    }
}
