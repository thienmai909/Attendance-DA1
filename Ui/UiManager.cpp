// UIManager.cpp
#include <UiManager.hpp>

// ── Forward declare các màn hình ──────────────────────────────────
static void screenSinhVien(AppManager& app);
static void screenLopHocPhan(AppManager& app);
static void screenDiemDanh(AppManager& app);
static void screenBaoCao(AppManager& app);

// ── Main menu ─────────────────────────────────────────────────────
void UIManager::run() {
    bool thoat = false;

    while (!thoat) {
        auto screen = ScreenInteractive::Fullscreen();

        auto entries = std::vector<std::string>{
            "Quan ly sinh vien",
            "Quan ly lop hoc phan",
            "Diem danh",
            "Thong ke & Bao cao",
            "Thoat"
        };

        int selected = 0;
        auto menu    = Menu(&entries, &selected);

        auto renderer = Renderer(menu, [&] {
            return vbox({
                text(" HE THONG DIEM DANH ") | bold | color(Color::Cyan) | center,
                separator(),
                menu->Render() | border | size(WIDTH, EQUAL, 40) | center,
                separator(),
                text(" [Enter] Chon   [q] Thoat ") | dim | center
            });
        }) | CatchEvent([&](Event event) {
            if (event == Event::Return) {
                screen.Exit();
                return true;
            }
            if (event == Event::Character('q')) {
                selected = 4;
                screen.Exit();
                return true;
            }
            return false;
        });

        screen.Loop(renderer);

        switch (selected) {
            case 0: screenSinhVien(_app);    break;
            case 1: screenLopHocPhan(_app);  break;
            case 2: screenDiemDanh(_app);    break;
            case 3: screenBaoCao(_app);      break;
            case 4: thoat = true;            break;
        }
    }
}

static void formThemSinhVien(AppManager& app) {
    auto screen = ScreenInteractive::Fullscreen();

    std::string maSV, tenSV;
    std::string thongBao;

    Component inputMaSV  = Input(&maSV,  "Nhap ma SV...");
    Component inputTenSV = Input(&tenSV, "Nhap ho ten...");

    auto btnThem = Button("Them", [&] {
        try {
            SinhVien sv(maSV, tenSV);
            app.getSVManager().them(sv);
            thongBao = "[OK] Da them: " + tenSV;
            maSV.clear();
            tenSV.clear();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
        }
    });

    auto btnThoat = Button("Quay lai", [&] { screen.Exit(); });

    auto layout = Container::Vertical({
        inputMaSV, inputTenSV,
        Container::Horizontal({ btnThem, btnThoat })
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            text(" THEM SINH VIEN ") | bold | color(Color::Green) | center,
            separator(),
            hbox({ text(" Ma SV  : "), inputMaSV->Render()  | flex }),
            hbox({ text(" Ho Ten : "), inputTenSV->Render() | flex }),
            separator(),
            hbox({
                btnThem->Render()  | size(WIDTH, EQUAL, 12),
                text("  "),
                btnThoat->Render() | size(WIDTH, EQUAL, 12),
            }) | center,
            separator(),
            text(" " + thongBao + " ") | color(
                thongBao.starts_with("[OK]") ? Color::Green : Color::Red
            ) | center
        }) | border | size(WIDTH, EQUAL, 50) | center;
    });

    screen.Loop(renderer);
}

static void formXoaSinhVien(AppManager& app) {
    auto screen = ScreenInteractive::Fullscreen();

    std::string maSV;
    std::string thongBao;

    Component inputMaSV = Input(&maSV, "Nhap ma SV can xoa...");

    auto btnXoa   = Button("Xoa", [&] {
        try {
            app.getSVManager().xoa(maSV);
            thongBao = "[OK] Da xoa: " + maSV;
            maSV.clear();
        } catch (const std::exception& e) {
            thongBao = "[ERR] " + std::string(e.what());
        }
    });

    auto btnThoat = Button("Quay lai", [&] { screen.Exit(); });

    auto layout = Container::Vertical({
        inputMaSV,
        Container::Horizontal({ btnXoa, btnThoat })
    });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            text(" XOA SINH VIEN ") | bold | color(Color::Red) | center,
            separator(),
            hbox({ text(" Ma SV : "), inputMaSV->Render() | flex }),
            separator(),
            hbox({
                btnXoa->Render()   | size(WIDTH, EQUAL, 12),
                text("  "),
                btnThoat->Render() | size(WIDTH, EQUAL, 12),
            }) | center,
            separator(),
            text(" " + thongBao + " ") | color(
                thongBao.starts_with("[OK]") ? Color::Green : Color::Red
            ) | center
        }) | border | size(WIDTH, EQUAL, 50) | center;
    });

    screen.Loop(renderer);
}

// ── Màn hình sinh viên ────────────────────────────────────────────
static void screenSinhVien(AppManager& app) {
    bool thoat = false;

    while (!thoat) {
        auto screen     = ScreenInteractive::Fullscreen();
        const auto& dsSV = app.getSVManager().getAll();

        auto entries = std::vector<std::string>{
            "Them sinh vien",
            "Xoa sinh vien",
            "Quay lai"
        };
        int selected = 0;
        auto menu    = Menu(&entries, &selected);

        auto makeTable = [&]() {
            std::vector<Element> rows;
            rows.push_back(
                hbox({
                    text("MaSV")  | bold | size(WIDTH, EQUAL, 12),
                    text(" | "),
                    text("Ho Ten") | bold | size(WIDTH, EQUAL, 22),
                    text(" | "),
                    text("Lop SH") | bold | size(WIDTH, EQUAL, 12),
                }) | color(Color::Yellow)
            );
            rows.push_back(separator());

            if (dsSV.empty()) {
                rows.push_back(text(" Chua co sinh vien. ") | dim | center);
            } else {
                for (const auto& sv : dsSV) {
                    rows.push_back(hbox({
                        text(sv.getMaSV())     | size(WIDTH, EQUAL, 12),
                        text(" | "),
                        text(sv.getTenSV())    | size(WIDTH, EQUAL, 22)
                    }));
                }
            }
            return vbox(rows);
        };

        auto renderer = Renderer(menu, [&] {
            return hbox({
                // Cột trái: bảng dữ liệu
                vbox({
                    text(" DANH SACH SINH VIEN ") | bold | color(Color::Green) | center,
                    separator(),
                    makeTable(),
                    separator(),
                    hbox({
                        text(" Tong: ") | dim,
                        text(std::to_string(dsSV.size())) | bold,
                    })
                }) | border | flex,

                // Cột phải: menu thao tác
                vbox({
                    text(" THAO TAC ") | bold | center,
                    separator(),
                    menu->Render(),
                    separator(),
                    text(" [Enter] Chon ") | dim | center,
                    text(" [q] Quay lai ") | dim | center,
                }) | border | size(WIDTH, EQUAL, 24)
            });
        }) | CatchEvent([&](Event event) {
            if (event == Event::Return) {
                screen.Exit();
                return true;
            }
            if (event == Event::Character('q')) {
                selected = 2;
                screen.Exit();
                return true;
            }
            return false;
        });

        screen.Loop(renderer);

        switch (selected) {
            case 0: formThemSinhVien(app); break;
            case 1: formXoaSinhVien(app);  break;
            case 2: thoat = true;          break;
        }
    }
}

// ── Màn hình lớp học phần ─────────────────────────────────────────
static void screenLopHocPhan(AppManager& app) {
    auto screen     = ScreenInteractive::Fullscreen();
    const auto& dsLHP = app.getLHPManager().getAll();

    auto renderer = Renderer([&] {
        std::vector<Element> rows;
        rows.push_back(
            hbox({
                text("MaLHP") | bold | size(WIDTH, EQUAL, 10),
                text(" | "),
                text("Ten LHP") | bold | size(WIDTH, EQUAL, 24),
                text(" | "),
                text("TC") | bold | size(WIDTH, EQUAL, 4),
                text(" | "),
                text("GiangVien") | bold | size(WIDTH, EQUAL, 10),
            }) | color(Color::Yellow)
        );
        rows.push_back(separator());

        if (dsLHP.empty()) {
            rows.push_back(text(" Chua co lop nao. ") | dim | center);
        } else {
            for (const auto& lhp : dsLHP) {
                rows.push_back(hbox({
                    text(lhp.getMaLHP())  | size(WIDTH, EQUAL, 10),
                    text(" | "),
                    text(lhp.getTenLHP()) | size(WIDTH, EQUAL, 24),
                    text(" | "),
                    text(std::to_string(lhp.getSoTC())) | size(WIDTH, EQUAL, 4),
                    text(" | "),
                    text(lhp.getMaGV())   | size(WIDTH, EQUAL, 10),
                }));
            }
        }

        return vbox({
            text(" QUAN LY LOP HOC PHAN ") | bold | color(Color::Yellow) | center,
            separator(),
            vbox(rows) | border,
            separator(),
            text(" [q] Quay lai ") | dim | center
        });
    }) | CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}

// ── Màn hình điểm danh ────────────────────────────────────────────
static void screenDiemDanh(AppManager& app) {
    auto screen = ScreenInteractive::Fullscreen();

    auto renderer = Renderer([&] {
        return vbox({
            text(" DIEM DANH ") | bold | color(Color::Magenta) | center,
            separator(),
            text(" (Dang phat trien...) ") | dim | center,
            separator(),
            text(" [q] Quay lai ") | dim | center
        }) | border;
    }) | CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}

// ── Màn hình báo cáo ──────────────────────────────────────────────
static void screenBaoCao(AppManager& app) {
    auto screen = ScreenInteractive::Fullscreen();

    auto renderer = Renderer([&] {
        return vbox({
            text(" THONG KE & BAO CAO ") | bold | color(Color::Blue) | center,
            separator(),
            text(" (Dang phat trien...) ") | dim | center,
            separator(),
            text(" [q] Quay lai ") | dim | center
        }) | border;
    }) | CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}